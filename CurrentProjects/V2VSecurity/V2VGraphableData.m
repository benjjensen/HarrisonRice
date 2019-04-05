function V2VGraphableData(testPoint, stepSize, averageWidth)
% V2VGraphableData
% Takes the data from a V2V test point that is arranged with respect to
% time and converts it to data that is arranged according to transmitter
% location.
%
% Author: Nathan Nelson

    addpath('Functions');

    % The file where the Tx GPS data is stored.
    txGPSFilename = 'Data/GPSData/Tx-gps-xcsv.txt';

    % The starting location, ending location, and step size.
    % The starting and ending location were determined by looking at all of the
    % data captures and determining where they overlap.
    minD = 0;
    maxD = 228.9;
    deltaD = stepSize;

    % In order for this to work, we need to assign a particular location to be
    % equal to zero. However, this location will probably be between our GPS
    % data points; what we need to do for each test point is find the GPS data
    % point that is closest to location zero and determine the location of that
    % GPS data point. Then we'll move all of our GPS data points so that the
    % correct data point is at the correct location.

    if averageWidth == 1
        allCarriersFilename = sprintf('Data/CorrectlyAveragedData/test-point-%s_jagged.mat', testPoint);
        middle32Filename = sprintf('Data/CorrectlyAveragedData/test-point-%s_jagged-mid32.mat', testPoint);
    else
        allCarriersFilename = sprintf('Data/CorrectlyAveragedData/test-point-%s_averaged-%d.mat', testPoint, averageWidth);
        middle32Filename = sprintf('Data/CorrectlyAveragedData/test-point-%s_averaged-%d-mid32.mat', testPoint, averageWidth);
    end
    
    if testPoint == 'F'
        metadataFilename = 'Data/Metadata/test-point-F-V2V_2019-02-21__13-59-52-meta.txt';
        rawDataFilename = '/media/V2V/2019-02-21_data-collection/test-point-F-V2V_processed.mat';
        
        referenceIndex = 53;
        referenceDistance = 2.5;
    elseif testPoint == 'G'
        metadataFilename = 'Data/Metadata/test-point-G-V2V_2019-02-21__14-04-26-meta.txt';
        rawDataFilename = '/media/V2VData02/2019-02-21_data-collection/test-point-G-V2V_processed.mat';
        
        referenceIndex = 1;
        referenceDistance = 0;
    elseif testPoint == 'H'
        metadataFilename = 'Data/Metadata/test-point-H-V2V_2019-02-21__14-07-35-meta.txt';
        rawDataFilename = '/media/V2V/2019-02-21_data-collection/test-point-H-V2V_processed.mat';
        
        referenceIndex = 4;
        referenceDistance = 0.08;
    elseif testPoint == 'I'
        metadataFilename = 'Data/Metadata/test-point-I-V2V_2019-02-21__14-10-46-meta.txt';
        rawDataFilename = '/media/V2V/2019-02-21_data-collection/test-point-I-V2V_processed.mat';
        
        referenceIndex = 10;
        referenceDistance = 6.23;
    elseif testPoint == 'J'
        metadataFilename = 'Data/Metadata/test-point-J-V2V_2019-02-21__14-15-49-meta.txt';
        rawDataFilename = '/media/V2V/2019-02-21_data-collection/test-point-J-V2V_processed.mat';
        
        referenceIndex = 2;
        referenceDistance = 3.25;
    else
        printf('Error: unkown test point %s', testPoint);
        return;
    end
    
    [RxDistances, blocks, startingTime] = ReadGPSDataPositions(metadataFilename);
    TxDistances = ReadTxGPSLocations(txGPSFilename, startingTime, max(size(RxDistances)));
    if testPoint == 'G'
        load('/media/V2VData02/2019-02-21_data-collection/test-point-G-V2V_2019-02-21__14-04-26.mat', 'samplesStoredBeforeEachGPSFix');
        blocks = samplesStoredBeforeEachGPSFix / 2^19;
        clear samplesStoredBeforeEachGPSFix;
    end

    % Right now, TxDistances contain the distance that the transmitter has
    % traveled from when we started recording data.
    % Shift TxDistances so that it instead contains the distance that the
    % transmitter has traveled from location 0.
    TxDistances = TxDistances - TxDistances(referenceIndex) + referenceDistance;
    
    dataMatFile = matfile(rawDataFilename);
    info = whos(dataMatFile, 'y');
    totalSamples = max(info.size);
    clear dataMatFile
    clear info
    
    pwelchBoundaries = GetPwelchBoundaries(TxDistances, blocks, deltaD, minD, maxD, totalSamples);
    if averageWidth ~= 1
        pwelchBoundaries = GetAveragedPwelchBoundaries(pwelchBoundaries, averageWidth);
    end
    
    PwelchOnFrameBoundaries(rawDataFilename, allCarriersFilename, ...
        middle32Filename, pwelchBoundaries);
end
