function data = readV2VData(filename)
    file_info = dir(filename);
    length_of_data_file = file_info.bytes / 2;

    data_file_id = fopen(filename);
    
    fseek(data_file_id, 1000000000, 'bof');
    
    data = fread(data_file_id, 200000000, 'unsigned short');
    
    fclose(data_file_id);
end