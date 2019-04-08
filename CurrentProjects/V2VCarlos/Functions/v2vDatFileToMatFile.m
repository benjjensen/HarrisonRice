function v2vDatFileToMatFile(datFilename, sampleRate)
% Convert a V2V .dat file to a .mat matlab data file.
% Author: Nathan Nelson
    DAT_PRECISION = 'unsigned short';
    SAMPLES_PER_ITERATION = 50000000;
    
    matFilename = getMatFilenameFromDatFilename(datFilename);
    
    if exist(matFilename, 'file') == 2
        fprintf('%s already exists.\n', matFilename);
        return;
    end
    
    mat = matfile(matFilename,'Writable',true);
    
    metaFilename = getMetaFilenameFromDatFilename(datFilename);
    metadata = fileread(metaFilename);
    
    lines = strsplit(metadata, '\n');
    numLines = max(size(lines));
    lineIdx = 1;
    while lineIdx <= numLines
        line = lines{lineIdx};
        words = strsplit(line);
        if strcmp(words{1}, 'name')
            mat.name = words{2};
        elseif strcmp(words{1}, 'date')
            mat.date = words{2};
        elseif strcmp(words{1}, 'time')
            mat.time = words{2};
        elseif strcmp(words{1}, 'duration')
            duration = str2double(words{2});
            mat.duration = duration;
        elseif strcmp(words{1}, 'notes')
            notes = line(7:end);
            if contains(notes,'`')
                notes = notes(1:end-1);
            else
                lineIdx = lineIdx + 1;
                while lineIdx <= numLines
                    line = lines{lineIdx};
                    if contains(line, '`')
                        notes = [notes, newline, line(1:end-1)];
                        break;
                    else
                        notes = [notes, newline, line];
                    end
                    lineIdx = lineIdx + 1;
                end
            end
            mat.notes = notes;
        elseif strcmp(words{1}, 'positions')
            positionCount = str2double(words{2});
            
            blocks = zeros(positionCount, 1);
            latitudes = zeros(positionCount, 1);
            longitudes = zeros(positionCount, 1);
            altitudes = zeros(positionCount, 1);
            for positionIdx = 1:positionCount
                line = lines{lineIdx + positionIdx};
                words = strsplit(line, {' ' ','});
                
                blocks(positionIdx) = str2double(words{3});
                
                lat = str2double(words{5});
                lon = str2double(words{6});
                
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

                latitudes(positionIdx) = latDeg + latFrac;
                longitudes(positionIdx) = lonDeg + lonFrac;
                
                altitudes(positionIdx) = str2double(words{7});
            end
            mat.latitudes = latitudes;
            mat.longitudes = longitudes;
            mat.altitudes = altitudes;
            
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
            mat.distances = distances;
            
            lineIdx = lineIdx + positionIdx;
        end
        lineIdx = lineIdx + 1;
    end
    
    samplesStoredBeforeEachGPSFix = blocks * 2^19;
    
    approxCorrectSampleCount = duration * sampleRate;
    
    fileInfo = dir(datFilename);
    actualSampleCount = floor(fileInfo.bytes/2);
    
    extraSamplesAtBeginning = max(0, actualSampleCount-approxCorrectSampleCount);
    
    samplesStoredBeforeEachGPSFix = samplesStoredBeforeEachGPSFix - ...
        extraSamplesAtBeginning;
    
    % TODO: what if the 2nd element of this array is less than zero? What
    % if it's positive but much smaller than it ought to be? IOW, what if
    % it took more than a second to catch up with the real-time data?
    samplesStoredBeforeEachGPSFix(1) = 0;
    
    mat.samplesStoredBeforeEachGPSFix = samplesStoredBeforeEachGPSFix;
    
    dat = fopen(datFilename);
    fseek(dat, extraSamplesAtBeginning*2, 'bof');
    
    index = 1;
    
    % Read in and save a small number of samples at a time; we can't read
    % too many at a time because they won't all fit in memory.
    while ~feof(dat)
        [data, count] = fread(dat, SAMPLES_PER_ITERATION, DAT_PRECISION);
        if count ~= 0
            mat.data(index:index+count-1,1) = data(:,1);
            index = index + count;
        end
    end
    
    fclose(dat);
end

function matFilename = getMatFilenameFromDatFilename(datFilename)
    filenameLength = length(datFilename);
    matFilename = datFilename;
    matFilename(filenameLength-2:filenameLength) = 'mat';
end

function metaFilename = getMetaFilenameFromDatFilename(datFilename)
    metaFilename = strcat(datFilename(1:end-4), '-meta.txt');
end