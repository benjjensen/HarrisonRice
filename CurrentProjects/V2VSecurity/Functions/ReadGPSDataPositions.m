function [distances, blocks] = ReadGPSDataPositions(filename)
    file = fopen(filename);
    
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
        blocks(idx) = str2double(words(3));
        
        lat = char(words(5));
        lat = str2double(lat(1:end-1));
        
        lon = char(words(6));
        lon = str2double(lon(1:end-1));
        
        latDeg = fix(lat/100);
        latMins = mod(abs(lat), 100);
        
        lonDeg = fix(lon/100);
        lonMins = mod(abs(lon), 100);
        
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