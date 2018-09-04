clearvars;

base_filename = 'multithreaded_2018-08-14__14-01-36';
filename = sprintf('/media/V2V/%s.dat', base_filename);
standard_deviation_multiple_threshold = 9;
samples_per_block = 524288;

%data = readBatched_DAQshortSamples_host(filename);
s = dir(filename);
length_of_data_file = s.bytes / 2;
clear s;
data_file_id = fopen(filename);
%out = fread(FID, length_of_data_file, 'unsigned short');

data_per_iteration = 100000000;
iterations = ceil(length_of_data_file / data_per_iteration);

indices_first_iteration = (1:(data_per_iteration - 1))';
indices_not_first_iteration = (1:data_per_iteration)';

total_break_count = 0;
% 10,000 breaks should be more than enough
all_breaks = zeros(10000, 2);
for iteration = 1:iterations
    if iteration == iterations
        % Read all of the remaining data if there's less than data_per_iteration
        % remaining:
        data = fread(data_file_id, data_per_iteration, 'unsigned short');
    elseif iteration == 1
        data = fread(data_file_id, data_per_iteration, 'unsigned short');
    else
        data = fread(data_file_id, data_per_iteration + 1, 'unsigned short');
    end
    
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
    
    if iteration < iterations
        % We want to read the last datum of this iteration in the next
        % iteration so that we don't leave out the difference between the last
        % datum of this iteration and the first datum of the next iteration.
        fseek(data_file_id, -2, 'cof');
    end
end
% all_breaks = all_breaks(1:total_break_count, :);
% 
% for break_number = 1:total_break_count
%     break_size = all_breaks(break_number, 1);
%     break_index = all_breaks(break_number, 2);
%     
%     break_block = floor(break_index / samples_per_block);
%     break_sample_in_block = break_index - samples_per_block * break_block;
%     
%     fprintf('Break size %d at index %d (sample %d in block %d)\n', ...
%         break_size, break_index, break_sample_in_block, break_block);
% end
% 
% breaks = all_breaks;
% out_filename = sprintf('%s-breaks.mat', base_filename)
% save(out_filename, 'breaks');
% 
% data_file_id = fopen(filename);
% 
% for break_number = 1:min(total_break_count, 50)
%     break_index = all_breaks(break_number, 2);
%     
%     starting_file_position = max((break_index - 499) * 2, 0);
%     fseek(data_file_id, starting_file_position, 'bof');
%     
%     data = fread(data_file_id, 1000, 'unsigned short');
%     
%     figure(break_number); clf;
%     plot(data);
%     grid on;
% end
% 
% fclose(data_file_id);


% mean_value = mean(data);
% differences = diff(data);
% mean_difference = mean(differences);
% absolute_differences = abs(differences);
% mean_absolute_difference = mean(absolute_differences);
% standard_deviation = std(absolute_differences);
% 
% [max_absolute_difference, max_absolute_difference_index] = max(absolute_differences);
% % absolute_differences(max_absolute_difference_index) = [];
% % [max_absolute_difference, max_absolute_difference_index] = max(absolute_differences);
% 
% fprintf('Maximum absolute difference %d found at index %d of file %s\n', ...
%     max_absolute_difference, max_absolute_difference_index, filename);
% 
% figure(1); clf
% plot(1:2000,data(max_absolute_difference_index - 999:max_absolute_difference_index + 1000) - mean_value,'.-');
% grid on;
% 
% figure(2); clf
% histogram(differences);
% 
% figure(3); clf
% histogram(absolute_differences);