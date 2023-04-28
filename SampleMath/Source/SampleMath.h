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

#ifndef SAMPLE_MATH_H_INCLUDED
#define SAMPLE_MATH_H_INCLUDED

#include <ProcessorHeaders.h>

/*
 * Allows adding, subtracting, multiplying and dividing constants or channels from selected channels.
 *
 * @see GenericProcessor
 */

// corresponds to indices of ComboBox choices
enum Operation
{
    ADD = 0,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    SUM,
    MEAN,
    VECTOR_SUM
};

enum Mode
{
    CONSTANT = 0,
    CHANNEL
};


class SampleMathSettings {
public:
    Operation operation;
    float constant;
    int channel;
    Mode mode;
};

class SampleMath : public GenericProcessor
{
    friend class SampleMathEditor;

public:
    SampleMath();
    ~SampleMath();

    AudioProcessorEditor* createEditor() override;

    void process(AudioSampleBuffer& continuousBuffer) override;

    void updateSettings() override;

    void parameterValueChanged(Parameter* param) override;

private:
    static bool opIsBinary(Operation op);

    StreamSettings<SampleMathSettings> settings;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleMath);
};

#endif // SAMPLE_MATH_H_INCLUDED