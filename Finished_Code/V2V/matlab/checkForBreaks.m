clearvars;

base_filename = 'reserved_cores_2018-08-16__14-41-44';

standard_deviation_multiple_threshold = 10;
samples_per_block = 524288;
bytes_per_sample = 2;
data_file_precision = 'unsigned short';
data_per_iteration = 100000000;
number_breaks_to_graph = 50;
data_per_graph = 1000;
graph_center_offset = 499;

COL_BREAK_INDEX = 2;
COL_BREAK_SIZE = 1;
BREAK_COLS = 2;



in_filename = sprintf('/media/V2V/%s.dat', base_filename);
file_info = dir(in_filename);
length_of_data_file = file_info.bytes / bytes_per_sample;

data_file_id = fopen(in_filename);

iterations = ceil(length_of_data_file / data_per_iteration);

total_break_count = 0;
% Preallocating space for 10,000 breaks should be more than enough.
all_breaks = zeros(10000, BREAK_COLS);

for iteration = 1:iterations
    if iteration == 1
        data = fread(data_file_id, data_per_iteration, data_file_precision);
    else
        data = fread(data_file_id, data_per_iteration + 1, data_file_precision);
    end
    % We want to read the last datum of this iteration in the next
    % iteration so that we don't leave out the difference between the last
    % datum of this iteration and the first datum of the next iteration.
    fseek(data_file_id, -1 * bytes_per_sample, 'cof');
    
    absolute_differences = abs(diff(data));
    
    if iteration == 1
        % Calculate the threshold that defines the boundary between normal
        % differences and breaks in the data.
        % No need to recalculate this every time, but it does need to be
        % calculated.
        mean_absolute_difference = mean(absolute_differences);
        absolute_difference_standard_deviation = std(absolute_differences);
    
        threshold = mean_absolute_difference + ...
            standard_deviation_multiple_threshold * absolute_difference_standard_deviation;
    end
    
    logical_differences_above_threshold = absolute_differences >= threshold;
    
    % This column vector contains the indices of the differences that are
    % above the threshold.
    indices_differences_above_threshold = find(logical_differences_above_threshold);
    
    % This column vector contains the differences that are above the
    % threshold.
    differences_above_threshold = ...
        absolute_differences(logical_differences_above_threshold);
    
    index_offset = (iteration - 1) * data_per_iteration;
    
    % Subtract one from the index offset (if it isn't zero) because the
    % first iteration only includes data_per_iteration - 1 differences,
    % whereas all other iterations include data_per_iteration differences.
    if index_offset > 0
        index_offset = index_offset - 1;
    end
    
    indices_differences_above_threshold = ...
        indices_differences_above_threshold + index_offset;
    
    break_count_this_iteration = size(indices_differences_above_threshold, 1);
    
    % Puts the breaks into a break_count_this_iteration x 2 matrix, where
    % the first column is the size of each break and the second column is
    % the location of each break.
    breaks_this_iteration = cat(2, differences_above_threshold, ...
        indices_differences_above_threshold);
    
    % Add the breaks we found to the list of breaks.
    if(break_count_this_iteration > 0)
        all_breaks((total_break_count + 1):(total_break_count + break_count_this_iteration), :) = ...
            breaks_this_iteration;
        total_break_count = total_break_count + break_count_this_iteration;
    end
end
% Trim the all_breaks matrix down to its real size (from 10000x2, which
% is what we preallocated it to be).
all_breaks = all_breaks(1:total_break_count, :);

% Print out information about each break.
for break_number = 1:total_break_count
    break_size = all_breaks(break_number, COL_BREAK_SIZE);
    break_index = all_breaks(break_number, COL_BREAK_INDEX);
    
    break_block = floor(break_index / samples_per_block);
    break_sample_in_block = break_index - samples_per_block * break_block;
    
    fprintf('Break size %d at index %d (sample %d in block %d)\n', ...
        break_size, break_index, break_sample_in_block, break_block);
end

% Save the breaks to a file.
breaks = all_breaks;
out_filename = sprintf('%s-breaks.mat', base_filename);
save(out_filename, 'breaks');

% Graph the first number_breaks_to_graph breaks so the user can see them.
for break_number = 1:min(total_break_count, number_breaks_to_graph)
    break_index = all_breaks(break_number, COL_BREAK_INDEX);
    
    starting_file_position = max((break_index - graph_center_offset) * bytes_per_sample, 0);
    fseek(data_file_id, starting_file_position, 'bof');
    
    data = fread(data_file_id, data_per_graph, data_file_precision);
    
    figure(break_number); clf;
    plot(data);
    grid on;
end

fclose(data_file_id);