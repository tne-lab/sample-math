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
    desiredWidth = 200;

    addComboBoxParameterEditor("Operation", 10, 20);
    addComboBoxParameterEditor("Mode", 100, 20);
    addTextBoxParameterEditor("Constant", 10, 70);
    addSelectedChannelsParameterEditor("Channel", 10, 70);

    Mode mode = Mode((int) getProcessor()->getParameter("Mode")->getValue());
    updateParameterVisibility(mode);
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
