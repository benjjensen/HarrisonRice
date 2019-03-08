function [distances, blocks, startingTime] = ReadGPSDataPositions(filename)
% Read in the distances traveled and data positions recorded in the file.
% Takes a metadata file and reads in information about the capture that the
% metadata describes.
%
% Returns three things:
% distances - An array of distance values, where distances(t) represents
%  the distance traveled since time 0.
% blocks - An array of block values, where blocks(t) represents the number
%  of blocks written since time 0.
% startingTime - A string representation of the time that the capture
%  started.
%
% Author: Nathan Nelson
    file = fopen(filename);
    
    % Skip all of the lines of the file until the start of the positions
    % list:
    line = fgetl(file);
    while ~strcmp(line(1:5), 'notes')
        line = fgetl(file);
    end
    while ~contains(line,'`')
        line = fgetl(file);
    end
    line = fgetl(file);
    while ~strcmp(line(1:9), 'positions')
        line = fgetl(file);
    end
    
    positionCount = str2double(line(11:end));
    blocks = zeros(positionCount, 1);
    latitudes = zeros(positionCount, 1);
    longitudes = zeros(positionCount, 1);
    
    for idx = 1:positionCount
        line = fgetl(file);
        words = split(line);
        if idx == 1
            % Take note of the first time data point.
            startingTime = char(words(2));
        end
        
        blocks(idx) = str2double(words(3));
        
        lat = char(words(5));
        lat = str2double(lat(1:end-1));
        
        lon = char(words(6));
        lon = str2double(lon(1:end-1));
        
        % lat and lon are of the form (degrees * 100) + minutes
        % Convert them to just degrees:
        latDeg = fix(lat/100);
        latMins = mod(abs(lat), 100);
        
        lonDeg = fix(lon/100);
        lonMins = mod(abs(lon), 100);
        
        % 60 minutes per degree:
        latFrac = latMins/60;
        lonFrac = lonMins/60;
        
        if latDeg < 0
            latFrac = -latFrac;
        end
        if lonDeg < 0
            lonFrac = -lonFrac;
        end
        
        latitudes(idx) = latDeg + latFrac;
        longitudes(idx) = lonDeg + lonFrac;
    end
    
    r = 6.371e6; % radius of earth in meters
    cumulativeDistance = 0; % cumulative distance traveled
    
    latitudes = deg2rad(latitudes);
    longitudes = deg2rad(longitudes);
    
    distances = zeros(positionCount, 1);
    % For each gps point, calculate the distance traveled from the previous
    % point and then note the cumulative distance traveled:
    for idx = 2:positionCount
        % This is the formula for distance between two points given the
        % gps coordinates for those two points.
        iterativeDistance = 2 * r * ...
            asin(sqrt((sin((latitudes(idx) - latitudes(idx-1))/2))^2 + ...
            cos(latitudes(idx-1))*cos(latitudes(idx))* ...
            (sin((longitudes(idx) - longitudes(idx-1))/2))^2));
        cumulativeDistance = cumulativeDistance + iterativeDistance;
        distances(idx) = cumulativeDistance;
    end
    
    fclose(file);
end