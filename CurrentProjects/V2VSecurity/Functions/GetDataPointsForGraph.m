function data = GetDataPointsForGraph(distances, blocks, pwelchedData, deltaD, minD, maxD)
% Given location and data with respect to time, get data with respect to location.
% 
% distances is an array where distances(t) represents the physical 
%  location/distance from a specific point at time t
% blocks is an array where blocks(t) represents the position within the
%  data set at time t; that is, the data located at the index specified in
%  blocks(t) was recorded at time t. A value of 1 represents 1 megabyte
%  offset from the start of the original .dat file.
% pwelchedData is an array of data
% deltaD is the step size--smaller deltaD results in more data points being
%  returned from this function
% minD is the starting location
% maxD is the ending location
%
% Author: Nathan Nelson
    % Convert the positions in the data file in blocks to positions in
    % frames.
    % 2^19 samples (at 100 MSamples/s) per 1 block; 1 sample (at 20
    % MSamples/s) per 5 samples (at 100 MSamples/s); 1 frame per 14,912
    % samples (at 20 MSamples/s).
    frames = blocks * 2^19 / 5 / 14912;
    
    numIterations = floor((maxD-minD)/deltaD+1);
    frameIndices = zeros(1,numIterations);
    currentIteration = 1;
    idx = 1;
    
    % Set this up so that we don't have a problem if the starting location
    % is less than our first location.
    lastD = distances(idx);
    DIntervalWidth = 1;
    lastFrame = frames(idx);
    frameIntervalWidth = 0;
    
    for D = minD:deltaD:maxD
        % Find the first known location that is greater than D, the target
        % location for this iteration:
        while distances(idx) <= D
            idx = idx + 1;
            lastD = distances(idx-1);
            nextD = distances(idx);
            DIntervalWidth = nextD-lastD;
            
            lastFrame = frames(idx-1);
            nextFrame = frames(idx);
            frameIntervalWidth = nextFrame - lastFrame;
        end
        % D is probably not exactly on one of the known locations;
        % calculate the proportion of the distance from D to the previous
        % known location and the distance between the previous and next
        % known locations.
        % (In other words, assume that the location changed linearly
        % between the two known points and find the time when the location
        % was equal to D.)
        proportion = (D-lastD) / DIntervalWidth;
        
        % Find the position in the data file that corresponds to the time
        % when we were at location D.
        % (Again, assume that the position in the data file changed
        % linearly between the two known points.)
        frameIndex = lastFrame + proportion*frameIntervalWidth;
        if frameIndex == 0
            frameIndex = 1;
        end
        
        frameIndices(currentIteration) = frameIndex;
        currentIteration = currentIteration + 1;
    end
    frameIndices = frameIndices(1:currentIteration-1);
    % Round to the nearest integer.
    frameIndices = round(frameIndices, 0);
    
    data = pwelchedData(frameIndices, :);
end