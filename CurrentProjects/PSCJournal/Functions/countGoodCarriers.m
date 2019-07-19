function [aveNumGoodCarriers] = countGoodCarriers(signal, noise, threshold)

    [numCarriers, numSamples] = size(signal);
    numGoodCarriers = zeros(numSamples, 1);
    
    signalDb = mag2db(signal);
    noiseDb = mag2db(noise);
    
    for sample = 1:numSamples
        numGood = 0;
        for carrier = 1:numCarriers
            if ((signalDb(carrier, sample) - noiseDb(1, sample))  >= threshold)
                numGood = numGood + 1;
            end
        end
        numGoodCarriers(sample,1) = numGood;
    end
    
    aveNumGoodCarriers = mean(numGoodCarriers);

end
