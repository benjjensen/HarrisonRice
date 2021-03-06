function [aveNumGoodCarriers, numGoodCarriers] = countGoodCarriers(signal, noise, threshold, startCar, endCar)

    [numCarriers, numSamples] = size(signal);
    numGoodCarriers = zeros(numSamples, 1);
    
    signalDb = mag2db(signal);
    noiseDb = mag2db(noise);
    
    for sample = 1:numSamples
        numGood = 0;
        for carrier = startCar:endCar
            if ((signalDb(carrier, sample) - noiseDb(1, sample))  >= threshold)
                numGood = numGood + 1;
            end
        end
        numGoodCarriers(sample,1) = numGood;
    end
    
    aveNumGoodCarriers = mean(numGoodCarriers);

end
