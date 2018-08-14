clearvars;
filename = '/home/adm85/UltraviewDAQcode/SimpleUltraSetup/Release/not_continuous.dat';
data = readBatched_DAQshortSamples_host(filename);

mean_value = mean(data);
differences = diff(data);
mean_difference = mean(differences);
absolute_differences = abs(differences);
mean_absolute_difference = mean(absolute_differences);

[max_absolute_difference, max_absolute_difference_index] = max(absolute_differences);

fprintf('Maximum absolute difference %d found at index %d of file %s\n', ...
    max_absolute_difference, max_absolute_difference_index, filename);

figure(1); clf
plot(1:2000,data(max_absolute_difference_index - 999:max_absolute_difference_index + 1000) - mean_value,'.-');
grid on;