function scaledData = ScaleRefSignalToSumToOne(data,noiseOnOddCarriers)
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
    
    averagedRefSignal = zeros(numDataSets,numCarriers);
    for i = 1:numDataSets
        for j = 1:numFrames
            if numDataSets > 1
                averagedRefSignal(i,:) = averagedRefSignal(i,:) + data(i,:,j);
            else
                averagedRefSignal(i,:) = averagedRefSignal(i,:) + data(:,j);
            end
        end
        averagedRefSignal(i,:) = averagedRefSignal(i,:) ./ numFrames;
    end
    
    for i = 1:numDataSets
        if noiseOnOddCarriers
            signalSum = sum(averagedRefSignal(i,2:2:numCarriers));
        else
            signalSum = sum(data(i,1:2:numCarriers));
        end
        scaledData(i,:) = averagedRefSignal(i,:) ./ signalSum;
    end
end

