function [carrierArray] = carrierReliability(signal, noise, threshold)
%carrierReliability - Calculates how often carriers are above a threshold
% Converts the signal and noise to dB
% Checks each carrier across every sample and determines how often that
%    carrier is above a given threshold
% Returns an array containing the result as a fraction of total samples

[numCarriers, numSamples] = size(signal);
carrierArray = zeros(numCarriers, 1);

signalDb = mag2db(signal);
noiseDb = mag2db(noise);

for carrier = 1:numCarriers
    count = 0;
    for sample = 1:numSamples
        if ( (signalDb(carrier, sample) - noiseDb(1, sample)) >= threshold)
            count = count + 1;
        end
    end
    carrierArray(carrier, 1) = (count / numSamples);
end

end

