% V2IGraphableData
% Takes the data from a V2I test point that is arranged with respect to
% time and converts it to data that is arranged according to receiver
% location.
%
% Author: Nathan Nelson

addpath('Functions');

% The files where the data is stored. Change these if you want to do a
% different test point.
metadataFilename = 'Data/Metadata/test-point-B-V2V_2019-02-21__13-35-21-meta.txt';
pwelchedDataFilename = 'Data/PwelchedData/test-point-B-V2V_pwelched-mid32.mat';
outputFilename = 'Data/GraphPwelchedData/test-point-B_graph-pwleched-data.mat';

% In order for this to work, we need to assign a particular location to be
% equal to zero. However, this location will probably be between our GPS
% data points; what we need to do for each test point is find the GPS data
% point that is closest to location zero and determine the location of that
% GPS data point. Then we'll move all of our GPS data points so that the
% correct data point is at the correct location.
referenceIndex = 34;
referenceDistance = 1.1;

% The step size.
deltaD = 0.1;



[RxDistances, blocks] = ReadGPSDataPositions(metadataFilename);

% Right now, RxDistances contain the distance that the receiver has
% traveled from when we started recording data.
% Shift RxDistances so that it instead contains the distance that the
% receiver has traveled from location 0.
RxDistances = RxDistances - RxDistances(referenceIndex) + referenceDistance;

% Calculate the starting and ending position so that 0 is at the center
% of the interval we'll be looking at.
minD = min(RxDistances);
minD = deltaD * fix(minD / deltaD);

maxD = max(RxDistances);
maxD = deltaD * fix(maxD / deltaD);

if abs(minD) > maxD
    minD = -maxD;
else
    maxD = -minD;
end

% Get the data points with respect to receiver location and save them to
% the proper file.
load(pwelchedDataFilename);
graphPwelchedData = GetDataPointsForGraph(RxDistances, blocks, pwelched,...
    deltaD, minD, maxD);
save(outputFilename, 'graphPwelchedData');