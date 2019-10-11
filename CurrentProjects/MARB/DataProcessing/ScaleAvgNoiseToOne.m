function scaledData = ScaleAvgNoiseToOne(data, noiseOnOddCarriers)
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
    [x, y, z] = size(data);
    if z > 1
        numDataSets = x;
        numCarriers = y;
        numFrames = z;
    else
        numDataSets = 1;
        numCarriers = x;
        numFrames = y;
    end
    for i = 1:numDataSets
        for j = 1:numFrames
            if numDataSets == 1
                if noiseOnOddCarriers
                    noiseAvg = mean(data(1:2:numCarriers,j));
                else
                    noiseAvg = mean(data(2:2:numCarriers,j));
                end
                scaledData(:,j) = data(:,j) ./ noiseAvg;
            else
                if noiseOnOddCarriers
                    noiseAvg = mean(data(i,1:2:numCarriers,j));
                else
                    noiseAvg = mean(data(i,2:2:numCarriers,j));
                end
                scaledData(i,:,j) = data(i,:,j) ./ noiseAvg;
            end
        end
    end
    
end

