function out = readBatched_DAQshortSamples_host(path)
%     path = 'DAQshortSamples_host';
    if exist(path)
        FID = fopen(path);
        myLength = 1500000;
        out = fread(FID,myLength,'unsigned short');
    else
        display(['Cant Open File' path])
        return
    end
end

