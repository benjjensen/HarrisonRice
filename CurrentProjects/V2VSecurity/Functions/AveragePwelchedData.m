function averaged = AveragePwelchedData(data, averageLength)
    before = floor((averageLength-1)/2);
    after = ceil((averageLength-1)/2);
    
    dims = size(data);
    originalFrameCount = dims(1);
    width = dims(2);
    
    averaged = zeros(originalFrameCount+averageLength-1, width);
    
    for idx = 1-after:originalFrameCount+before
        first = max(1, idx-before);
        last = min(originalFrameCount, idx+after);
        
        newFrame = sum(data(first:last,:), 1) / averageLength;
        averaged(idx+after, :) = newFrame;
    end
end