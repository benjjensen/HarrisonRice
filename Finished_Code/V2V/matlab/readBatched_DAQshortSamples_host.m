function out = readBatched_DAQshortSamples_host(path)
%     path = 'DAQshortSamples_host';
    if exist(path)
        FID = fopen(path);
        myLength = real(fread(FID,1,'int32'));
        out = fread(FID,myLength,'unsigned short');
    else
        display(['Cant Open File' path])
        return
    end
end

