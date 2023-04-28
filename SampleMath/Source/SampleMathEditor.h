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

#ifndef SAMPLE_MATH_EDITOR_H_INCLUDED
#define SAMPLE_MATH_EDITOR_H_INCLUDED

#include <EditorHeaders.h>
#include "SampleMath.h"

class SampleMathEditor : public GenericEditor
{
public:
    SampleMathEditor(GenericProcessor* parentNode);
    ~SampleMathEditor();

    void updateParameterVisibility(Mode mode);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleMathEditor);
};

#endif // SAMPLE_MATH_EDITOR_H_INCLUDED
