load('harrison.mat');
for row = 1:29
    for run = 1:33
        for loc = 19:2:110
            noise_floor = noise_floor + harrison(row,run,loc+1);
        end
    end
end