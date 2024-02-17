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

/** Holds settings for one stream's filters*/
void DataConverterSettings::createFilters(int numChannels, float sampleRate_, double scaling, double offset)
{
    sampleRate = sampleRate_;

    filters.clear();

    for (int n = 0; n < numChannels; ++n)
        filters.add(new LinearEquation());

    updateFilters(scaling, offset);
}

void DataConverterSettings::updateFilters(double scaling, double offset)
{
    for (int n = 0; n < filters.size(); n++)
    {
        setFilterParameters(scaling, offset, n);
    }
}

void DataConverterSettings::setFilterParameters(double scaling, double offset, int channel)
{
    filters[channel]->scaling = scaling;
    filters[channel]->offset = offset;
}

DataConverter::DataConverter()
    : GenericProcessor("Data Converter")
{
    addFloatParameter(Parameter::STREAM_SCOPE, "scaling", "Multiplies the input by this value", 1, -1e10, 1e10, false);
    addFloatParameter(Parameter::STREAM_SCOPE, "offset", "Adds this value after the above scaling", 0, -1e10, 1e10, false);
    addMaskChannelsParameter(Parameter::STREAM_SCOPE, "Channels", "Channels to filter for this stream");
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
    settings.update(getDataStreams());

    for (auto stream : getDataStreams())
    {
        settings[stream->getStreamId()]->createFilters(
            stream->getChannelCount(),
            stream->getSampleRate(),
            (*stream)["scaling"],
            (*stream)["offset"]
        );
    }
}

void DataConverter::parameterValueChanged(Parameter* param)
{
    
    uint16 currentStream = param->getStreamId();

    settings[currentStream]->updateFilters(
        (*getDataStream(currentStream))["scaling"],
        (*getDataStream(currentStream))["offset"]
    );
}

void DataConverter::process(AudioBuffer<float>& buffer)
{

    for (auto stream : getDataStreams())
    {

        if ((*stream)["enable_stream"])
        {
            DataConverterSettings* streamSettings = settings[stream->getStreamId()];

            const uint16 streamId = stream->getStreamId();
            const uint32 numSamples = getNumSamplesInBlock(streamId);

            for (auto localChannelIndex : *((*stream)["Channels"].getArray()))
            {
                int globalChannelIndex = getGlobalChannelIndex(stream->getStreamId(), (int) localChannelIndex);

                float* ptr = buffer.getWritePointer(globalChannelIndex);

                streamSettings->filters[localChannelIndex]->apply(ptr, numSamples);

            }
        }
    }

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
