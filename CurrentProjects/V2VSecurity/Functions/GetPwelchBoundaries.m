function frameBoundaries = GetPwelchBoundaries(distances, blocks, deltaD, minD, maxD, totalSamples)
    filePositions = blocks * 2^19 / 5;
    sampleRate = 20e6;
    
    DIndex = 1;
    positionIndex = 1;
    
    positionCount = floor((maxD-minD)/deltaD) + 1;
    timesAtPosition = zeros(positionCount, 1);
    for position = minD:deltaD:maxD
        while distances(DIndex) <= position
            DIndex = DIndex + 1;
        end
        if positionIndex == 1
            startingFilePosition = filePositions(DIndex-1);
            startingFileTime = DIndex-2;
        end
        lastD = distances(DIndex-1);
        nextD = distances(DIndex);
        
        proportion = (position-lastD) / (nextD-lastD);
        time = DIndex-2 + proportion;
        timesAtPosition(positionIndex) = time;
        positionIndex = positionIndex + 1;
    end
    
    frameCenters = ...
        (timesAtPosition-startingFileTime)*sampleRate + startingFilePosition;

    frameBoundaries = zeros(positionCount, 2);
    for frameIndex = 1:positionCount
        frameCenter = frameCenters(frameIndex);
        
        if frameIndex ~= 1
            frameStart = mean(frameCenters(frameIndex-1:frameIndex));
        end
        
        if frameIndex ~= positionCount
            frameEnd = mean(frameCenters(frameIndex:frameIndex+1));
        else
            frameEnd = frameCenter + (frameCenter-frameStart);
        end
        
        if frameIndex == 1
            frameStart = frameCenter - (frameEnd-frameCenter);
        end
        
        frameStart = max(1, round(frameStart,0));
        frameEnd = min(totalSamples, round(frameEnd,0));
        frameStart = min(frameStart, frameEnd);
        
        frameWidth = frameEnd - frameStart + 1;
        while mod(frameWidth,64)
            if mod(frameWidth, 2)
                frameEnd = frameEnd-1;
            else
                frameEnd = frameEnd-1;
                frameStart = frameStart+1;
            end
            frameWidth = frameEnd - frameStart + 1;
        end
        
        frameBoundaries(frameIndex, :) = [frameStart frameEnd];
    end
end