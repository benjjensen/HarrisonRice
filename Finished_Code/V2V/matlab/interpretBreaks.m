function interpretBreaks(base_filename, graph_breaks)
    samples_per_block = 524288;
    number_breaks_to_graph = 60;
    data_per_graph = 3000;
    graph_center_offset = 1499;
    bytes_per_sample = 2;
    data_file_precision = 'unsigned short';

    COL_BREAK_INDEX = 2;
    COL_BREAK_SIZE = 1;
    COL_BREAK_BLOCK = 3;
    COL_BREAK_SAMPLE_IN_BLOCK = 4;
    BREAK_COLS = 4;
    
    breaks_filename = sprintf('%s-breaks.mat', base_filename);
    breaks = [];
    load(breaks_filename, 'breaks');
    break_count = size(breaks, 1);
    
    break_columns = size(breaks, 2);
    
    if(break_columns < BREAK_COLS)
        breaks(:, COL_BREAK_BLOCK) = floor(breaks(COL_BREAK_INDEX) / samples_per_block);
        breaks(:, COL_BREAK_SAMPLE_IN_BLOCK) = breaks(COL_BREAK_INDEX) - ...
        samples_per_block * breaks(COL_BREAK_BLOCK);
        
        save(breaks_filename, 'breaks');
    end
    
    for break_number = 1:break_count
        break_size = breaks(break_number, COL_BREAK_SIZE);
        break_index = breaks(break_number, COL_BREAK_INDEX);

        break_block = floor(break_index / samples_per_block);
        break_sample_in_block = break_index - samples_per_block * break_block;

        fprintf('Break size %d at index %d (sample %d in block %d)\n', ...
            break_size, break_index, break_sample_in_block, break_block);
    end

    if graph_breaks == true
        data_filename = sprintf('/media/V2V/%s.dat', base_filename);
        data_file_id = fopen(data_filename);

        % Graph the first number_breaks_to_graph breaks so the user can see them.
        for break_number = 1:min(break_count, number_breaks_to_graph)
            break_index = breaks(break_number, COL_BREAK_INDEX);

            starting_file_position = max((break_index - graph_center_offset) * bytes_per_sample, 0);
            fseek(data_file_id, starting_file_position, 'bof');

            data = fread(data_file_id, data_per_graph, data_file_precision);

            figure(break_number); clf;
            plot(data);
            grid on;
        end

        fclose(data_file_id);
    end
end