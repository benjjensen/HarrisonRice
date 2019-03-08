function v2vDatFileToMatFile(datFilename)
% Convert a V2V .dat file to a .mat matlab data file.
% Author: Nathan Nelson
    DAT_PRECISION = 'unsigned short';
    SAMPLES_PER_ITERATION = 50000000;
    
    matFilename = getMatFilenameFromDatFilename(datFilename);
    
    if exist(matFilename, 'file') == 2
        fprintf('%s already exists.', matFilename);
        return;
    end
    
    dat = fopen(datFilename);
    mat = matfile(matFilename,'Writable',true);
    
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