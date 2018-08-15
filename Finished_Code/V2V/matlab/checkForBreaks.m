clearvars;

base_filename = 'multithreaded_2018-08-15__15-38-53';
filename = sprintf('/media/V2V/%s.dat', base_filename);
standard_deviation_multiple_threshold = 10;
samples_per_block = 524288;

s = dir(filename);
length_of_data_file = s.bytes / 2;
clear s;
data_file_id = fopen(filename);

data_per_iteration = 100000000;
iterations = ceil(length_of_data_file / data_per_iteration);

indices_first_iteration = (1:(data_per_iteration - 1))';
indices_not_first_iteration = (1:data_per_iteration)';

total_break_count = 0;
% Preallocating 10,000 breaks should be more than enough
all_breaks = zeros(10000, 2);
for iteration = 1:iterations
    if iteration == 1
        data = fread(data_file_id, data_per_iteration, 'unsigned short');
    else
        data = fread(data_file_id, data_per_iteration + 1, 'unsigned short');
    end
    % We want to read the last datum of this iteration in the next
    % iteration so that we don't leave out the difference between the last
    % datum of this iteration and the first datum of the next iteration.
    fseek(data_file_id, -2, 'cof');
    
    absolute_differences = abs(diff(data));
    
    if iteration == 1
        % No need to recalculate this every time, but it does need to be
        % calculated.
        mean_absolute_difference = mean(absolute_differences);
        absolute_difference_standard_deviation = std(absolute_differences);
    
        threshold = mean_absolute_difference + ...
            standard_deviation_multiple_threshold * absolute_difference_standard_deviation;
    end
    
    logical_differences_above_threshold = absolute_differences >= threshold;
    differences_above_threshold = ...
        absolute_differences(logical_differences_above_threshold);
    
    if iteration == 1
        indices_differences_above_threshold = ...
            indices_first_iteration(logical_differences_above_threshold);
    else
        indices_differences_above_threshold = ...
            indices_not_first_iteration(logical_differences_above_threshold);
    end
    
    index_offset = (iteration - 1) * data_per_iteration;
    if index_offset > 0
        index_offset = index_offset - 1;
    end
    
    indices_differences_above_threshold = ...
        indices_differences_above_threshold + index_offset;
    
    break_count_this_iteration = size(indices_differences_above_threshold, 1);
    
    breaks_this_iteration = cat(2, differences_above_threshold, ...
        indices_differences_above_threshold);
    
    if(break_count_this_iteration > 0)
        all_breaks((total_break_count + 1):(total_break_count + break_count_this_iteration), :) = ...
            breaks_this_iteration;
        total_break_count = total_break_count + break_count_this_iteration;
    end
end
all_breaks = all_breaks(1:total_break_count, :);

for break_number = 1:total_break_count
    break_size = all_breaks(break_number, 1);
    break_index = all_breaks(break_number, 2);
    
    break_block = floor(break_index / samples_per_block);
    break_sample_in_block = break_index - samples_per_block * break_block;
    
    fprintf('Break size %d at index %d (sample %d in block %d)\n', ...
        break_size, break_index, break_sample_in_block, break_block);
end

breaks = all_breaks;
out_filename = sprintf('%s-breaks.mat', base_filename);
save(out_filename, 'breaks');

for break_number = 1:min(total_break_count, 50)
    break_index = all_breaks(break_number, 2);
    
    starting_file_position = max((break_index - 499) * 2, 0);
    fseek(data_file_id, starting_file_position, 'bof');
    
    data = fread(data_file_id, 1000, 'unsigned short');
    
    figure(break_number); clf;
    plot(data);
    grid on;
end

fclose(data_file_id);