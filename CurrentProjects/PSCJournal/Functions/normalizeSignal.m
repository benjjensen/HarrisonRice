function [normSignal_linear, aveNoise] = normalizeSignal(data, startCar, endCar)

    [~, numSamples] = size(data);   % Get size 
    
    
    signal = data(2:2:end, :);      % Separate the signal from noise
    noiseFloor = data(1:2:end, :);
    
    for sample = 1:numSamples              % Average noise floor
        aveNoise(:, sample) = mean(noiseFloor(startCar:endCar, sample));
    end
    
    signal = signal ./ aveNoise;   % Normalize to account for AGC
    signal = sqrt(signal);      % Find the G's
    signal = signal ./ max(max(signal));    % Normalize by the best gain 
    signal = signal .^2; % Return g^2 
    
    normSignal_linear = signal;
end