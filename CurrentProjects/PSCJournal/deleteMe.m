clear; close all;

numMinutes = 0.25;               % Change this,
framesPerMinute = 8000;
totalFrames = framesPerMinute*numMinutes;

nameOfDataArray = 'test';        % this, 
test = zeros(2048, totalFrames); % and shift+enter change this

startCarrierPlot = 1;            % Edge carriers for stem plot
endCarrierPlot = 64;
averageStartCarrier = 11;        % Edge carriers when averaging signal 
averageEndCarrier = 54;   

%% Setup

plutoradiosetup();
rx = sdrrx('Pluto');
rx.RadioID = 'usb:0';
rx.CenterFrequency = 1250e6;
rx.BasebandSampleRate = 20e6;
rx.SamplesPerFrame = 32*64;
rx.OutputDataType = 'double';
rx.ShowAdvancedProperties = true;

%%
while (1)
    % Clears the queue of old data
    dataToIgnore = zeros(2048,1);   % Any significance to these numbers?
    for delay = 1:100
        dataToIgnore(:,1) = rx();
    end
    
    % Gathers new data
    for loop = 1:totalFrames
        test(:,loop) = rx();
    end
    
    % Saves the new data under the name stored in "nameOfDataArray"
    save(string(nameOfDataArray) + '.mat',string(nameOfDataArray));
    
    %% Process Data (Pwelch + FFT)
    dataArray = test;
    numSamples = totalFrames;
    numCarriers = (endCarrierPlot - startCarrierPlot + 1);
    
    [processedData_linear, processedData_db] = processArray(dataArray, numSamples);
    
    %% Separate noise and signal
    
        % Separate out the signal
    signal_linear = processedData_linear(2:2:end, :);      % Signal carriers are even (this time)
        % Separate out the noise floor and then average it
    noise_linear = processedData_linear(1:2:end, :); % Noise carriers are odd
    noise_average = mean(noise_linear(startCarrierPlot:endCarrierPlot, :)); % Only average useful ones
    
    signal_db = mag2db(signal_linear);
    noise_db = mag2db(noise_average);
        % Divide the signal by noise to isolate the signal (subtraction
        % used for dB)
    signal_linear = signal_linear ./ noise_average;
    signal_db = signal_db - noise_db;
    %%
    
    signal_midCarriers_linear = signal_linear(startCarrierPlot:endCarrierPlot, :);
    
    
    for carrier = 1:numCarriers
        signal_average_linear(carrier) = mean(signal_midCarriers_linear(carrier, :)); %%%%% correct?
    end
    average = mean((signal_average_linear(averageStartCarrier:averageEndCarrier)));
    
    figure(1)
    stem(mag2db(signal_average_linear));
    title({"Average (" + string(averageStartCarrier) + " to " + string(averageEndCarrier) + "): " + string(mag2db(average)); ...
           "Average in linear: " + string(average)});
    drawnow;
    beep;
end





