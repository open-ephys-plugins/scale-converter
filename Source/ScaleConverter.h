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

#ifndef ScaleConverter_H_DEFINED
#define ScaleConverter_H_DEFINED

#include <ProcessorHeaders.h>

class LinearEquation
{
public:
    /** Constructor -- sets default values*/
    LinearEquation() {}

    /** Holds the scaling factor for this channel*/
    float scaling;

    /** Holds the offset for this channel*/
    float offset;

    /** Applies the equation to the input buffer*/
    void apply (float* data, int numSamples)
    {
        for (int n = 0; n < numSamples; ++n)
        {
            data[n] = data[n] * scaling + offset;
        }
    }
};

/** Holds settings for one stream's filters*/

class ScaleConverterSettings
{
public:
    /** Constructor -- sets default values*/
    ScaleConverterSettings() {}

    /** Holds the sample rate for this stream*/
    float sampleRate;

    /** Holds the filters for one stream*/
    OwnedArray<LinearEquation> filters;

    /** Creates new filters when input settings change*/
    void createFilters (int numChannels, float sampleRate, double scaling, double offset);

    /** Updates filters when parameters change*/
    void updateFilters (double scaling, double offset);

    /** Sets filter parameters for one channel*/
    void setFilterParameters (double scaling, double offset, int channel);
};

class ScaleConverter : public GenericProcessor
{
public:
    /** The class constructor, used to initialize any members. */
    ScaleConverter();

    /** The class destructor, used to deallocate memory */
    ~ScaleConverter();

    /** Registers the parameters for the ScaleConverter */
    void registerParameters() override;

    /** If the ScaleConverter has a custom editor, this method must be defined to instantiate it. */
    AudioProcessorEditor* createEditor() override;

    /** Called every time the settings of an upstream plugin are changed.
		Allows the ScaleConverter to handle variations in the channel configuration or any other parameter
		passed through signal chain. The ScaleConverter can use this function to modify channel objects that
		will be passed to downstream plugins. */
    void updateSettings() override;

    /** Called whenever a parameter's value is changed (called by GenericProcessor::setParameter())*/
    void parameterValueChanged (Parameter* param) override;

    /** Defines the functionality of the ScaleConverter.
		The process method is called every time a new data buffer is available.
		Visualizer plugins typically use this method to send data to the canvas for display purposes */
    void process (AudioBuffer<float>& buffer) override;

    /** Handles events received by the ScaleConverter
		Called automatically for each received event whenever checkForEvents() is called from
		the plugin's process() method */
    void handleTTLEvent (TTLEventPtr event) override;

    /** Handles spikes received by the ScaleConverter
		Called automatically for each received spike whenever checkForEvents(true) is called from
		the plugin's process() method */
    void handleSpike (SpikePtr spike) override;

    /** Handles broadcast messages sent during acquisition
		Called automatically whenever a broadcast message is sent through the signal chain */
    void handleBroadcastMessage (const String& message, const int64 systemTimeMillis) override;

    /** Saving custom settings to XML. This method is not needed to save the state of
		Parameter objects */
    void saveCustomParametersToXml (XmlElement* parentElement) override;

    /** Load custom settings from XML. This method is not needed to load the state of
		Parameter objects*/
    void loadCustomParametersFromXml (XmlElement* parentElement) override;

private:
    StreamSettings<ScaleConverterSettings> settings;
};

#endif