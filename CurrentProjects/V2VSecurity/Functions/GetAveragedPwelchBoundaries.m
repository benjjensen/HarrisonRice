function averagedPwelchBoundaries = GetAveragedPwelchBoundaries(boundaries, averageWidth)
    averagedPwelchBoundaries = boundaries;
    frameCount = max(size(boundaries));
    for idx = 1:frameCount
        startFrame = ceil(idx - (averageWidth-1)/2);
        endFrame = ceil(idx + (averageWidth-1)/2);
        startFrame = max(1, startFrame);
        endFrame = min(frameCount, endFrame);
        
        startBoundary = boundaries(startFrame);
        endBoundary = boundaries(endFrame);
        
        width = endBoundary - startBoundary + 1;
        while mod(width,64)
            if mod(width, 2)
                endBoundary = endBoundary-1;
            else
                endBoundary = endBoundary-1;
                startBoundary = startBoundary+1;
            end
            width = endBoundary - startBoundary + 1;
        end
        averagedPwelchBoundaries(idx, :) = [boundaries(startFrame) boundaries(endFrame)];
    end
end