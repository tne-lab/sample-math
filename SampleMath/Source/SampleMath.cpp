/*
------------------------------------------------------------------

This file is part of a plugin for the Open Ephys GUI
Copyright (C) 2018 Translational NeuroEngineering Laboratory

------------------------------------------------------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "SampleMath.h"
#include "SampleMathEditor.h"

SampleMath::SampleMath()
    : GenericProcessor  ("Samp Math")
    , operation         (ADD)
    , useChannel        (false)
    , constant          (0)
    , selectedChannel   (-1)
{
    setProcessorType(PROCESSOR_TYPE_FILTER);
}

SampleMath::~SampleMath() {}

AudioProcessorEditor* SampleMath::createEditor()
{
    editor = new SampleMathEditor(this);
    return editor;
}

void SampleMath::process(AudioSampleBuffer& continuousBuffer)
{
    Operation currOp = operation;
    bool isBinary = opIsBinary(currOp);
    int currSelectedChannel = selectedChannel;
    bool reallyUseChannel = useChannel && isBinary;

    const float* rp;
    if (reallyUseChannel)
    {
        rp = continuousBuffer.getReadPointer(currSelectedChannel);
    }

    // iterate over active channels
    Array<int> activeChannels = editor->getActiveChannels();
    int numActiveChannels = activeChannels.size();
    bool selectedChannelIsActive = false;

    int numValues;
    if (numActiveChannels == 0)
    {
        return;
    }
    else
    {
        numValues = getNumSamples(activeChannels[0]);
    }

    Array<float> naryResult;
    if (!isBinary)
    {
        // pre-loop through channels to calculate n-ary function result over selected channels
        naryResult.insertMultiple(0, 0.0f, numValues);
        float* resultPtr = naryResult.getRawDataPointer();

        for (int chan : activeChannels)
        {
            const float* sourcePtr = continuousBuffer.getReadPointer(chan);
            switch (currOp)
            {
            case SUM:
                FloatVectorOperations::add(resultPtr, sourcePtr, numValues);
                break;

            case MEAN:
                FloatVectorOperations::addWithMultiply(resultPtr, sourcePtr,
                    1.0f / numActiveChannels, numValues);
                break;

            case VECTOR_SUM:
                FloatVectorOperations::addWithMultiply(resultPtr, sourcePtr,
                    sourcePtr, numValues);
                //FloatVectorOperations::
                
                for (int i = 0; i < numValues; ++i)
                {
                    resultPtr[i] = std::sqrt(resultPtr[i]);
                }

            default:
                jassertfalse;
                break;
            }
        }
    }

    for (int chan : activeChannels)
    {
        if (reallyUseChannel && chan == currSelectedChannel)
        {
            selectedChannelIsActive = true;
            continue; // process separately at end
        }

        float* wp = continuousBuffer.getWritePointer(chan);

        switch (currOp)
        {
        case ADD:
            if (reallyUseChannel)
            {
                FloatVectorOperations::add(wp, rp, numValues);
            }
            else
            {
                FloatVectorOperations::add(wp, constant, numValues);
            }
            break;

        case SUBTRACT:
            if (reallyUseChannel)
            {
                FloatVectorOperations::subtract(wp, rp, numValues);
            }
            else
            {
                FloatVectorOperations::add(wp, -constant, numValues);
            }
            break;

        case MULTIPLY:
            if (reallyUseChannel)
            {
                FloatVectorOperations::multiply(wp, rp, numValues);
            }
            else
            {
                FloatVectorOperations::multiply(wp, constant, numValues);
            }
            break;

        case DIVIDE:
            if (reallyUseChannel)
            {
                for (int i = 0; i < numValues; ++i)
                {
                    wp[i] /= rp[i];
                }
            }
            else
            {
                FloatVectorOperations::multiply(wp, 1.0f / constant, numValues);
            }
            break;

        case SUM:
        case MEAN:
        case VECTOR_SUM:
            FloatVectorOperations::copy(wp, naryResult.getRawDataPointer(), numValues);
            break;
        
        default: 
            jassertfalse;
            break;
        }
    }

    if (selectedChannelIsActive)
    {
        int numValues = getNumSamples(currSelectedChannel);
        float* wp = continuousBuffer.getWritePointer(currSelectedChannel);

        switch (currOp)
        {
        case ADD:
            FloatVectorOperations::add(wp, wp, numValues);
            break;

        case SUBTRACT:
            FloatVectorOperations::clear(wp, numValues);
            break;

        case MULTIPLY:
            FloatVectorOperations::multiply(wp, wp, numValues);
            break;

        case DIVIDE:
            FloatVectorOperations::fill(wp, 1.0f, numValues);
        }
    }
}

void SampleMath::setParameter(int parameterIndex, float newValue)
{
    switch (parameterIndex)
    {
    case OPERATION:
        operation = static_cast<Operation>(static_cast<int>(newValue));
        break;

    case USE_CHANNEL:
        if (newValue)
        {
            validateActiveChannels();
        }
        useChannel = static_cast<bool>(newValue);
        break;

    case CONSTANT:
        constant = newValue;
        break;

    case CHANNEL:
        int newChanNum = static_cast<int>(newValue);
        if (newChanNum == -1 || newChanNum >= getTotalDataChannels())
        {
            if (!CoreServices::getAcquisitionStatus())
            {
                selectedChannel = -1;
            }
            break;
        }

        validSubProcFullID = chanToFullID(newChanNum);
        if (useChannel)
        {
            validateActiveChannels();
        }
        selectedChannel = newChanNum;
        break;
    }
}

void SampleMath::updateSettings()
{
    // refresh selected channel, and if still valid, validate active channels if necessary
    int numChannels = getNumInputs();
    if (numChannels == 0)
    {
        selectedChannel = -1; // = none available
        return;
    }
    
    if (selectedChannel >= numChannels || selectedChannel == -1)
    {
        selectedChannel = 0; // reset to default
    }

    validSubProcFullID = chanToFullID(selectedChannel);
    if (useChannel)
    {
        validateActiveChannels();
    }
}

// private

void SampleMath::validateActiveChannels()
{
    Array<int> activeChannels = editor->getActiveChannels();
    int numChannels = getNumInputs();
    bool p, r, a, haveSentMessage = false;
    const String message = "Deselecting channels that don't match subprocessor of selected reference";
    for (int chan : activeChannels)
    {
        if (chan >= numChannels) // can happen during update if # of channels decreases
        {
            continue;
        }

        if (chanToFullID(chan) != validSubProcFullID)
        {
            if (!haveSentMessage)
            {
                CoreServices::sendStatusMessage(message);
            }
            editor->getChannelSelectionState(chan, &p, &r, &a);
            editor->setChannelSelectionState(chan - 1, false, r, a);
        }
    }
}

juce::uint32 SampleMath::chanToFullID(int chanNum) const
{
    const DataChannel* chan = getDataChannel(chanNum);
    uint16 sourceNodeID = chan->getSourceNodeID();
    uint16 subProcessorIdx = chan->getSubProcessorIdx();
    return getProcessorFullId(sourceNodeID, subProcessorIdx);
}

bool SampleMath::opIsBinary(Operation op)
{
    return !(op == SUM || op == MEAN || op == VECTOR_SUM);
}