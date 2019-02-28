function v2vDatFileToMatFile(datFilename)
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