function goodData = RemoveAttenuatedEdges(processedData, noiseOnOddCarriers, dataIsLinear)
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

    % Check if the data is linear, if so convert it to dB, if already in dB
    % leave it alone
    if dataIsLinear
        data = 10*log10(abs(processedData));
    else
        data = processedData;
    end
    
    % Identify the middle index of the data (rounds in the case of an odd
    % number of carriers), then ensures that the variable is pointing to a
    % noise carrier near the center frequency
    [carriers, frames] = size(data);
    halfpoint = round(carriers / 2);
    if mod(halfpoint,2) == 0 && noiseOnOddCarriers
        halfpoint = halfpoint - 1;
    end
    
    % Go through each frame, average the middle 10 or so values of the
    % noise to get an estimate of where the noise floor should be. Then use
    % that estimate to determine where the noise begins to be attenuated.
    % This isolates the usable carriers and eliminates the attenuated
    % carriers.
    noiseEstimate = zeros(frames,1);
    keepCarriers = zeros(carriers, frames);
    for i = 1:frames
        noiseEstimate(i, 1) = mean(data(halfpoint-10:2:halfpoint+10,i));
        maxNoiseRange = abs(max(data(halfpoint-10:2:halfpoint+10,i)));
        minNoiseRange = abs(min(data(halfpoint-10:2:halfpoint+10,i)));
        if maxNoiseRange < minNoiseRange
            tempval = minNoiseRange;
            minNoiseRange = maxNoiseRange;
            maxNoiseRange = tempval;
        end
        if noiseOnOddCarriers
            for j = 1:2:carriers
                if minNoiseRange <= abs(data(j,i))
                    if abs(data(j,i)) <= maxNoiseRange
                        keepCarriers(j,i) = 1;
                    end
                end
            end
        else
            for j = 2:2:carriers
                if minNoiseRange <= abs(data(j,i))
                    if abs(data(j,i)) <= maxNoiseRange
                        keepCarriers(j,i) = 1;
                    end
                end
            end
        end        
    end
    
    % If the carrier index is 'good' for more than half of the frames,
    % track the smallest and largest index to get the range of good
    % indices
    lowCarrier = 0;
    highCarrier = carriers;
    for i = 1:carriers
        sumOverFrames(i,1) = sum(keepCarriers(i,:));
        if sumOverFrames(i,1) > frames / 2
            if lowCarrier == 0
                lowCarrier = i;
            end
            highCarrier = i;
        end
    end
    
    % The 'good' carriers should include an equal amount of noise and
    % signal carriers, so by counting how many carriers are kept above and
    % below the center carrier, the side with the larger number is
    % truncated by one carrier to get that equal number of noise and signal
    % carriers
    noiseCarrierSpread = zeros(carriers,1);
    noiseCarrierSpread(lowCarrier:highCarrier,1) = 1;
    numCarriersBelowHalf = 0;
    numCarriersAboveHalf = 0;
    for i = 1:carriers
        if i < halfpoint
            if noiseCarrierSpread(i,1) == 1
                numCarriersBelowHalf = numCarriersBelowHalf + 1;
            end
        elseif i > halfpoint 
            if noiseCarrierSpread(i,1) == 1
                numCarriersAboveHalf = numCarriersAboveHalf + 1;
            end
        end
    end
    if numCarriersBelowHalf > numCarriersAboveHalf
        numCarriersBelowHalf = numCarriersBelowHalf - 1;
        lowCarrier = lowCarrier + 1;
    elseif numCarriersBelowHalf < numCarriersAboveHalf
        numCarriersAboveHalf = numCarriersAboveHalf - 1;
        highCarrier = highCarrier - 1;
    else
    end
    
    % Plots an example showing which carriers were kept, allowing for
    % visual verification of the result the code found
    stem(data(:,1));
    hold on
    maskedData = zeros(carriers,1);
    maskedData(lowCarrier:highCarrier,1) = data(lowCarrier:highCarrier,1);
    stem(maskedData, 'MarkerFaceColor','k');
    title({'Result from RemoveAttenuatedEdges.m', ...
        'Plot values are in dB for visual clarity'});
    hold off
        
    % Masks the original data with zeros for everything that is
    % attentuated, leaving the 'good' carriers untouched
    keepCarriers = zeros(carriers,frames);
    keepCarriers(lowCarrier:highCarrier,:) = 1;
    goodData = keepCarriers .* processedData;
end

