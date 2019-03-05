function data = GetDataPointsForGraph(distances, blocks, pwelchedData, deltaD, minD, maxD)
    frames = blocks * 2^19 / 5 / 14912;
    
    numIterations = floor((maxD-minD)/deltaD+1);
    frameIndices = zeros(1,numIterations);
    currentIteration = 1;
    idx = 1;
    for D = minD:deltaD:maxD
        while distances(idx) < D
            idx = idx + 1;
            lastD = distances(idx-1);
            nextD = distances(idx);
            DIntervalWidth = nextD-lastD;
            
            lastFrame = frames(idx-1);
            nextFrame = frames(idx);
            frameIntervalWidth = nextFrame - lastFrame;
        end
        proportion = (D-lastD) / DIntervalWidth;
        
        frameIndex = lastFrame + proportion*frameIntervalWidth;
        
        frameIndices(currentIteration) = frameIndex;
        currentIteration = currentIteration + 1;
    end
    frameIndices = frameIndices(1:currentIteration-1);
    frameIndices = round(frameIndices, 0);
    
    data = pwelchedData(frameIndices, :);
end