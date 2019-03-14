function V2VGraphableData(testPoint, stepSize, averageWidth)
% V2IGraphableData
% Takes the data from a V2I test point that is arranged with respect to
% time and converts it to data that is arranged according to receiver
% location.
%
% Author: Nathan Nelson

    addpath('Functions');

    if averageWidth == 1
        allCarriersFilename = sprintf('Data/CorrectlyAveragedData/test-point-%s_jagged.mat', testPoint);
        middle32Filename = sprintf('Data/CorrectlyAveragedData/test-point-%s_jagged-mid32.mat', testPoint);
    else
        allCarriersFilename = sprintf('Data/CorrectlyAveragedData/test-point-%s_averaged-%d.mat', testPoint, averageWidth);
        middle32Filename = sprintf('Data/CorrectlyAveragedData/test-point-%s_averaged-%d-mid32.mat', testPoint, averageWidth);
    end
    
    if testPoint == 'A'
        % The files where the data is stored. Change these if you want to do a
        % different test point.
        metadataFilename = 'Data/Metadata/test-point-A-V2V_2019-02-21__13-29-40-meta.txt';
        rawDataFilename = '/media/V2V/2019-02-21_data-collection/test-point-A-V2V_processed.mat';

        % In order for this to work, we need to assign a particular location to be
        % equal to zero. However, this location will probably be between our GPS
        % data points; what we need to do for each test point is find the GPS data
        % point that is closest to location zero and determine the location of that
        % GPS data point. Then we'll move all of our GPS data points so that the
        % correct data point is at the correct location.
        referenceIndex = 78;
        referenceDistance = 1.22;
    elseif testPoint == 'B'
        % The files where the data is stored. Change these if you want to do a
        % different test point.
        metadataFilename = 'Data/Metadata/test-point-B-V2V_2019-02-21__13-35-21-meta.txt';
        rawDataFilename = '/media/V2V/2019-02-21_data-collection/test-point-B-V2V_processed.mat';

        % In order for this to work, we need to assign a particular location to be
        % equal to zero. However, this location will probably be between our GPS
        % data points; what we need to do for each test point is find the GPS data
        % point that is closest to location zero and determine the location of that
        % GPS data point. Then we'll move all of our GPS data points so that the
        % correct data point is at the correct location.
        referenceIndex = 34;
        referenceDistance = 1.1;
    else
        printf('Error: unkown test point %s', testPoint);
        return;
    end

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
    
    dataMatFile = matfile(rawDataFilename);
    info = whos(dataMatFile, 'y');
    totalSamples = max(info.size);
    clear dataMatFile
    clear info
    
    pwelchBoundaries = GetPwelchBoundaries(RxDistances, blocks, deltaD, minD, maxD, totalSamples);
    if averageWidth ~= 1
        pwelchBoundaries = GetAveragedPwelchBoundaries(pwelchBoundaries, averageWidth);
    end
    
    PwelchOnFrameBoundaries(rawDataFilename, allCarriersFilename, ...
        middle32Filename, pwelchBoundaries);
end