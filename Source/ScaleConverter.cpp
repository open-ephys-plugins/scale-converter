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

#include "ScaleConverter.h"

#include "ScaleConverterEditor.h"

/** Holds settings for one stream's filters*/
void ScaleConverterSettings::createFilters(int numChannels, float sampleRate_, double scaling, double offset)
{
    sampleRate = sampleRate_;

    filters.clear();

    for (int n = 0; n < numChannels; ++n)
        filters.add(new LinearEquation());

    updateFilters(scaling, offset);
}

void ScaleConverterSettings::updateFilters(double scaling, double offset)
{
    for (int n = 0; n < filters.size(); n++)
    {
        setFilterParameters(scaling, offset, n);
    }
}

void ScaleConverterSettings::setFilterParameters(double scaling, double offset, int channel)
{
    filters[channel]->scaling = scaling;
    filters[channel]->offset = offset;
}

ScaleConverter::ScaleConverter()
    : GenericProcessor("Scale Converter")
{
}


ScaleConverter::~ScaleConverter()
{
}

void ScaleConverter::registerParameters()
{
    addFloatParameter(Parameter::STREAM_SCOPE, "scaling", "Scaling", "Multiplies the input by this value", "", 1, -1e32, 1e32, 0.0000001, false);
    addFloatParameter(Parameter::STREAM_SCOPE, "offset", "Offset", "Adds this value after the above scaling", "", 0, -1e32, 1e32, 0.0000001, false);
    addMaskChannelsParameter(Parameter::STREAM_SCOPE, "channels", "Channels", "Channels to filter for this stream");
    addCategoricalParameter(Parameter::STREAM_SCOPE, "threads", "Threads", "Number of threads to use", { "1", "8", "16", "32"}, 3);
}


AudioProcessorEditor* ScaleConverter::createEditor()
{
    editor = std::make_unique<ScaleConverterEditor>(this);
    return editor.get();
}


void ScaleConverter::updateSettings()
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

void ScaleConverter::parameterValueChanged(Parameter* param)
{
    
    uint16 currentStream = param->getStreamId();

    settings[currentStream]->updateFilters(
        (*getDataStream(currentStream))["scaling"],
        (*getDataStream(currentStream))["offset"]
    );
}

void ScaleConverter::process(AudioBuffer<float>& buffer)
{

    for (auto stream : getDataStreams())
    {

        if ((*stream)["enable_stream"])
        {
            ScaleConverterSettings* streamSettings = settings[stream->getStreamId()];

            const uint16 streamId = stream->getStreamId();
            const uint32 numSamples = getNumSamplesInBlock(streamId);

            for (auto localChannelIndex : *((*stream)["channels"].getArray()))
            {
                int globalChannelIndex = getGlobalChannelIndex(stream->getStreamId(), (int) localChannelIndex);

                float* ptr = buffer.getWritePointer(globalChannelIndex);

                streamSettings->filters[localChannelIndex]->apply(ptr, numSamples);

            }
        }
    }

}


void ScaleConverter::handleTTLEvent(TTLEventPtr event)
{

}


void ScaleConverter::handleSpike(SpikePtr spike)
{

}


void ScaleConverter::handleBroadcastMessage(const String& message, const int64 systemTimeMillis)
{

}


void ScaleConverter::saveCustomParametersToXml(XmlElement* parentElement)
{

}


void ScaleConverter::loadCustomParametersFromXml(XmlElement* parentElement)
{

}
