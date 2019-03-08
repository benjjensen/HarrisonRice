% V2VGraphableData
% Takes the data from a V2V test point that is arranged with respect to
% time and converts it to data that is arranged according to transmitter
% location.

% The files where the data is stored. Change these if you want to do a
% different test point.
metadataFilename = 'Data/Metadata/test-point-J-V2V_2019-02-21__14-15-49-meta.txt';
pwelchedDataFilename = 'Data/PwelchedData/test-point-J-V2V_pwelched-mid32.mat';
outputFilename = 'Data/GraphPwelchedData/test-point-J_graph-pwelched-data.mat';

% The file where the Tx GPS data is stored.
txGPSFilename = 'Data/GPSData/Tx-gps-xcsv.txt';

% The starting location, ending location, and step size.
% The starting and ending location were determined by looking at all of the
% data captures and determining where they overlap. The step size was
% chosen arbitrarily.
minD = 0;
maxD = 228.9;
deltaD = 0.1;

% In order for this to work, we need to assign a particular location to be
% equal to zero. However, this location will probably be between our GPS
% data points; what we need to do for each test point is find the GPS data
% point that is closest to location zero and determine the location of that
% GPS data point. Then we'll move all of our GPS data points so that the
% correct data point is at the correct location.
referenceIndex = 2;
referenceDistance = 3.25;

[RxDistances, blocks, startingTime] = ReadGPSDataPositions(metadataFilename);
TxDistances = ReadTxGPSLocations(txGPSFilename, startingTime, max(size(RxDistances)));

% Right now, TxDistances contain the distance that the transmitter has
% traveled from when we started recording data.
% Shift TxDistances so that it instead contains the distance that the
% transmitter has traveled from location 0.
TxDistances = TxDistances - TxDistances(referenceIndex) + referenceDistance;

load(pwelchedDataFilename); % pwelched
graphPwelchedData = GetDataPointsForGraph(TxDistances, blocks, pwelched, deltaD, minD, maxD);
save(outputFilename, 'graphPwelchedData');