function data = readV2VData(filename, offset, samples)
    data_file_id = fopen(filename);
    
    fseek(data_file_id, offset, 'bof');
    
    data = fread(data_file_id, samples, 'unsigned short');
    
    fclose(data_file_id);
end