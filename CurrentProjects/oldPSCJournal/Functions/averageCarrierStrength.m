function [carrierArray] = averageCarrierStrength(signal)
%averageCarrierStrength - Averages the strength of each carrier across all
% samples

    [numCarriers, numSamples] = size(signal);
    carrierArray = zeros(numCarriers, 1);

    % Switch for loops
    
    for carrier = 1:numCarriers
        signalStrength = 0;
        for sample = 1:numSamples
            signalStrength = signalStrength + signal(carrier, sample);
        end
        carrierArray(carrier) = mag2db(signalStrength / numSamples);
    end
          
end
