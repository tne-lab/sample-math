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

#include "SampleMathEditor.h"
#include <string> // stof
#include <cfloat> // limits

SampleMathEditor::SampleMathEditor(GenericProcessor* parentNode)
    : GenericEditor(parentNode)
{
    SampleMath* processor = static_cast<SampleMath*>(parentNode);
    desiredWidth = 200;

    addComboBoxParameterEditor(Parameter::STREAM_SCOPE, "Operation", 10, 30);
    addComboBoxParameterEditor(Parameter::STREAM_SCOPE, "Mode", 10, 60);
    addTextBoxParameterEditor(Parameter::STREAM_SCOPE, "Constant", 10, 90);
    addSelectedChannelsParameterEditor(Parameter::STREAM_SCOPE, "Channel", 10, 90);

  /*  Mode mode = Mode((int) processor->getParameter("Mode")->getValue());
    updateParameterVisibility(mode);*/
}

SampleMathEditor::~SampleMathEditor() {}


void SampleMathEditor::updateParameterVisibility(Mode mode)
{
    for (auto paramEditor : parameterEditors)
    {
        if (paramEditor->getParameterName().equalsIgnoreCase("Constant"))
        {
            paramEditor->setVisible(mode == CONSTANT);
        }
        else if (paramEditor->getParameterName().equalsIgnoreCase("Channel"))
        {
            paramEditor->setVisible(mode == CHANNEL);
        }
    }
}
