/*
------------------------------------------------------------------

This file is part of the Open Ephys GUI
Copyright (C) 2022 Open Ephys

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

#include "DataConverter.h"

#include "DataConverterEditor.h"


DataConverter::DataConverter()
    : GenericProcessor("Data Converter")
{

}


DataConverter::~DataConverter()
{

}


AudioProcessorEditor* DataConverter::createEditor()
{
    editor = std::make_unique<DataConverterEditor>(this);
    return editor.get();
}


void DataConverter::updateSettings()
{


}


void DataConverter::process(AudioBuffer<float>& buffer)
{

    checkForEvents(true);

}


void DataConverter::handleTTLEvent(TTLEventPtr event)
{

}


void DataConverter::handleSpike(SpikePtr spike)
{

}


void DataConverter::handleBroadcastMessage(String message)
{

}


void DataConverter::saveCustomParametersToXml(XmlElement* parentElement)
{

}


void DataConverter::loadCustomParametersFromXml(XmlElement* parentElement)
{

}
