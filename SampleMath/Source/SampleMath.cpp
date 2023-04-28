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
{
   addCategoricalParameter(Parameter::STREAM_SCOPE, "Operation", "The operation to use", { "+",  L"\u2212", L"\u00d7", L"\u00f7", "SUM", "MEAN", "VECTOR SUM" }, 0);
   addCategoricalParameter(Parameter::STREAM_SCOPE, "Mode", "Channel or constant", {"CONST", "CHAN"}, 0);
   addFloatParameter(Parameter::STREAM_SCOPE, "Constant", "The constant to use", 0, 0, std::numeric_limits<float>::max(), .001);
   addSelectedChannelsParameter(Parameter::STREAM_SCOPE, "Channel", "The continuous channel to use", 1);
}

SampleMath::~SampleMath() {}

AudioProcessorEditor* SampleMath::createEditor()
{
    editor = std::make_unique<SampleMathEditor>(this);
    return editor.get();
}

void SampleMath::process(AudioSampleBuffer& continuousBuffer)
{
    for (auto stream : getDataStreams())
    {
        const uint16 streamId = stream->getStreamId();
        const uint32 numValues = getNumSamplesInBlock(streamId);

        // Extract parameters
        SampleMathSettings* smSettings = settings[streamId];
        Operation currOp = smSettings->operation;
        int currSelectedChannel = smSettings->channel;
        Mode currMode = smSettings->mode;
        float constant = smSettings->constant;

        bool isBinary = opIsBinary(currOp);

        
        int numActiveChannels = continuousChannels.size();

        // Check that active channel is valid
        bool isValidChannel = currSelectedChannel > -1 && currSelectedChannel < numActiveChannels;
        bool useChannel = currMode == CHANNEL && isBinary && isValidChannel;

        bool selectedChannelIsActive = false;

        const float* rp;
        if (useChannel)
        {
            rp = continuousBuffer.getReadPointer(currSelectedChannel);
        }

        Array<float> naryResult;
        if (!isBinary)
        {
            // pre-loop through channels to calculate n-ary function result over selected channels
            naryResult.insertMultiple(0, 0.0f, numValues);
            float* resultPtr = naryResult.getRawDataPointer();

            for (auto chan : continuousChannels)
            {
                /*const float* sourcePtr = continuousBuffer.getReadPointer(chan->getLocalIndex());*/
                const float* sourcePtr = continuousBuffer.getReadPointer(chan->getGlobalIndex());
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

        for (auto chan : continuousChannels)
        {
            int index = chan->getLocalIndex();
            if (useChannel && index == currSelectedChannel)
            {
                selectedChannelIsActive = true;
                continue; // process separately at end
            }

            float* wp = continuousBuffer.getWritePointer(index);

            switch (currOp)
            {
            case ADD:
                if (useChannel)
                {
                    FloatVectorOperations::add(wp, rp, numValues);
                }
                else if (currMode == CONSTANT)
                {
                    FloatVectorOperations::add(wp, constant, numValues);
                }
                break;

            case SUBTRACT:
                if (useChannel)
                {
                    FloatVectorOperations::subtract(wp, rp, numValues);
                }
                else if (currMode == CONSTANT)
                {
                    FloatVectorOperations::add(wp, -constant, numValues);
                }
                break;

            case MULTIPLY:
                if (useChannel)
                {
                    FloatVectorOperations::multiply(wp, rp, numValues);
                }
                else if (currMode == CONSTANT)
                {
                    FloatVectorOperations::multiply(wp, constant, numValues);
                }
                break;

            case DIVIDE:
                if (useChannel)
                {
                    for (int i = 0; i < numValues; ++i)
                    {
                        wp[i] /= rp[i];
                    }
                }
                else if (currMode == CONSTANT)
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
}

void SampleMath::updateSettings()
{
    settings.update(getDataStreams());

    for (auto stream : getDataStreams())
    {
        // Update settings objects
        parameterValueChanged(stream->getParameter("Operation"));
        parameterValueChanged(stream->getParameter("Mode"));
        parameterValueChanged(stream->getParameter("Constant"));
        parameterValueChanged(stream->getParameter("Channel"));
    }
}


void SampleMath::parameterValueChanged(Parameter* param)
{
    const uint16 streamId = param->getStreamId();

    if (param->getName().equalsIgnoreCase("Channel"))
    {
        Array<var>* array = param->getValue().getArray();

        // Make sure there's a selected value
        if (array->size() > 0)
        {
            int localIndex = int(array->getFirst());
            int globalIndex = getDataStream(param->getStreamId())->getContinuousChannels()[localIndex]->getGlobalIndex();
            settings[streamId]->channel = globalIndex;
        }
        else
        {
            settings[streamId]->channel = -1;
        }
    }
    else if (param->getName().equalsIgnoreCase("Mode"))
    {
        Mode newMode = Mode((int)param->getValue());
        settings[streamId]->mode = newMode;

        ((SampleMathEditor*)getEditor())->updateParameterVisibility(newMode);
    }
    else if (param->getName().equalsIgnoreCase("Operation"))
    {
        Operation newOperation = Operation((int) param->getValue());
        settings[streamId]->operation = newOperation;
    }
    else if (param->getName().equalsIgnoreCase("Constant"))
    {
        settings[streamId]->constant = (float) param->getValue();
    }
}

bool SampleMath::opIsBinary(Operation op)
{
    return !(op == SUM || op == MEAN || op == VECTOR_SUM);
}