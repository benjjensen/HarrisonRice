close all;
threshold = 13;
tx2threshold = 28;
num_rows = 32;
num_runs = 26;
cam1 = zeros(num_rows*num_runs,1);
spot = 0;
for row = 1:num_rows
    for run = 1:num_runs
        num_carriers = 0;
        noise_floor = 0;
        count = 0;
%         for loc = 19:2:110
% 
%             noise_floor = noise_floor + camacho(row,run,loc+1);
%             count = count+1;
%         end
%         noise_floor = noise_floor/count;
%         for loc = 19:2:110
%             if noise_floor == 0
%                 num_carriers = nan;
%             else
%                 dif = camacho(row,run,loc) - noise_floor;
%                 if(dif > threshold)
%                     num_carriers = num_carriers + 1;
%                 end
%             end
%         end
        for loc = 1:2:127
            if camacho(row,run,loc) - camacho(row,run,loc+1) > threshold
                num_carriers = num_carriers + 1;
            end
        end
        spot = spot + 1;
        cam1(spot,1) = num_carriers;
    end
end
num_rows = 32;
num_runs = 26;
cam2 = zeros(num_rows*num_runs,1);
spot = 0;
for row = 1:num_rows
    for run = 1:num_runs
        num_carriers = 0;
        noise_floor = 0;
        count = 0;
%         for loc = 19:2:110
%             noise_floor = noise_floor + tx2camacho(row,run,loc+1);
%             count = count+1;
%         end
%         noise_floor = noise_floor/count;
%         for loc = 19:2:110
%             if noise_floor == 0
%                 num_carriers = nan;
%             else
%                 dif = tx2camacho(row,run,loc) - noise_floor;
%                 if(dif > tx2threshold)
%                     num_carriers = num_carriers + 1;
%                 end
%             end
%         end
        for loc = 1:2:127
            if tx2camacho(row,run,loc) - tx2camacho(row,run,loc+1) > tx2threshold
                num_carriers = num_carriers + 1;
            end
        end
        spot = spot + 1;
        cam2(spot,1) = num_carriers;
    end
end
num_rows = 32;
num_runs = 28;
chambers1 = zeros(num_rows*num_runs,1);
spot = 0;
for row = 1:num_rows
    for run = 1:num_runs
        num_carriers = 0;
        noise_floor = 0;
        count = 0;
%         for loc = 19:2:110
% 
%             noise_floor = noise_floor + chambers(row,run,loc+1);
%             count = count+1;
%         end
%         noise_floor = noise_floor/count;
%         for loc = 19:2:110
%             if noise_floor == 0
%                 num_carriers = nan;
%             else
%                 dif = chambers(row,run,loc) - noise_floor;
%                 if(dif > threshold)
%                     num_carriers = num_carriers + 1;
%                 end
%             end
%         end
        for loc = 1:2:127
            if chambers(row,run,loc) - chambers(row,run,loc+1) > threshold
                num_carriers = num_carriers + 1;
            end
        end
        spot = spot + 1;
        chambers1(spot,1) = num_carriers;
    end
end
num_rows = 32;
num_runs = 28;
chambers2 = zeros(num_rows*num_runs,1);
spot = 0;
for row = 1:num_rows
    for run = 1:num_runs
        num_carriers = 0;
        noise_floor = 0;
        count = 0;
%         for loc = 19:2:110
%             noise_floor = noise_floor + tx2chambers(row,run,loc+1);
%             count = count+1;
%         end
%         noise_floor = noise_floor/count;
%         for loc = 19:2:110
%             if noise_floor == 0
%                 num_carriers = nan;
%             else
%                 dif = tx2chambers(row,run,loc) - noise_floor;
%                 if(dif > tx2threshold)
%                     num_carriers = num_carriers + 1;
%                 end
%             end
%         end
        for loc = 1:2:127
            if tx2chambers(row,run,loc) - tx2chambers(row,run,loc+1) > tx2threshold
                num_carriers = num_carriers + 1;
            end
        end
        spot = spot + 1;
        chambers2(spot,1) = num_carriers;
    end
end
num_rows = 65;
num_runs = 61;
conference1 = zeros(num_rows*num_runs,1);
spot = 0;
for row = 1:num_rows
    for run = 1:num_runs
        num_carriers = 0;
        noise_floor = 0;
        count = 0;
%         for loc = 19:2:110
% 
%             noise_floor = noise_floor + conference(row,run,loc+1);
%             count = count+1;
%         end
%         noise_floor = noise_floor/count;
%         for loc = 19:2:110
%             if noise_floor == 0
%                 num_carriers = nan;
%             else
%                 dif = conference(row,run,loc) - noise_floor;
%                 if(dif > threshold)
%                     num_carriers = num_carriers + 1;
%                 end
%             end
%         end
        for loc = 1:2:127
            if conference(row,run,loc) - conference(row,run,loc+1) > threshold
                num_carriers = num_carriers + 1;
            end
        end
        spot = spot + 1;
        conference1(spot,1) = num_carriers;
    end
end
num_rows = 65;
num_runs = 61;
conference2 = zeros(num_rows*num_runs,1);
spot = 0;
for row = 1:num_rows
    for run = 1:num_runs
        num_carriers = 0;
        noise_floor = 0;
        count = 0;
%         for loc = 19:2:110
%             noise_floor = noise_floor + tx2conference(row,run,loc+1);
%             count = count+1;
%         end
%         noise_floor = noise_floor/count;
%         for loc = 19:2:110
%             if noise_floor == 0
%                 num_carriers = nan;
%             else
%                 dif = tx2conference(row,run,loc) - noise_floor;
%                 if(dif > tx2threshold)
%                     num_carriers = num_carriers + 1;
%                 end
%             end
%         end
        for loc = 1:2:127
            if tx2conference(row,run,loc) - tx2conference(row,run,loc+1) > tx2threshold
                num_carriers = num_carriers + 1;
            end
        end
        spot = spot + 1;
        conference2(spot,1) = num_carriers;
    end
end
num_rows = 29;
num_runs = 33;
harrison1 = zeros(num_rows*num_runs,1);
spot = 0;
for row = 1:num_rows
    for run = 1:num_runs
        num_carriers = 0;
        noise_floor = 0;
        count = 0;
%         for loc = 19:2:110
% 
%             noise_floor = noise_floor + harrison(row,run,loc+1);
%             count = count+1;
%         end
%         noise_floor = noise_floor/count;
%         for loc = 19:2:110
%             if noise_floor == 0
%                 num_carriers = nan;
%             else
%                 dif = harrison(row,run,loc) - noise_floor;
%                 if(dif > threshold)
%                     num_carriers = num_carriers + 1;
%                 end
%             end
%         end
        for loc = 1:2:127
            if harrison(row,run,loc) - harrison(row,run,loc+1) > threshold
                num_carriers = num_carriers + 1;
            end
        end
        spot = spot + 1;
        harrison1(spot,1) = num_carriers;
    end
end
num_rows = 30;
num_runs = 34;
harrison2 = zeros(num_rows*num_runs,1);
spot = 0;
for row = 1:num_rows
    for run = 1:num_runs
        num_carriers = 0;
        noise_floor = 0;
        count = 0;
%         for loc = 19:2:110
%             noise_floor = noise_floor + tx2harrison(row,run,loc+1);
%             count = count+1;
%         end
%         noise_floor = noise_floor/count;
%         for loc = 19:2:110
%             if noise_floor == 0
%                 num_carriers = nan;
%             else
%                 dif = tx2harrison(row,run,loc) - noise_floor;
%                 if(dif > tx2threshold)
%                     num_carriers = num_carriers + 1;
%                 end
%             end
%         end
        for loc = 1:2:127
            if tx2harrison(row,run,loc) - tx2harrison(row,run,loc+1) > tx2threshold
                num_carriers = num_carriers + 1;
            end
        end
        spot = spot + 1;
        harrison2(spot,1) = num_carriers;
    end
end
num_rows = 31;
num_runs = 26;
smalley1 = zeros(num_rows*num_runs,1);
spot = 0;
for row = 1:num_rows
    for run = 1:num_runs
        num_carriers = 0;
        noise_floor = 0;
        count = 0;
%         for loc = 19:2:110
% 
%             noise_floor = noise_floor + smalley(row,run,loc+1);
%             count = count+1;
%         end
%         noise_floor = noise_floor/count;
%         for loc = 19:2:110
%             if noise_floor == 0
%                 num_carriers = nan;
%             else
%                 dif = smalley(row,run,loc) - noise_floor;
%                 if(dif > threshold)
%                     num_carriers = num_carriers + 1;
%                 end
%             end
%         end
        for loc = 1:2:127
            if smalley(row,run,loc) - smalley(row,run,loc+1) > threshold
                num_carriers = num_carriers + 1;
            end
        end
        spot = spot + 1;
        smalley1(spot,1) = num_carriers;
    end
end
num_rows = 32;
num_runs = 26;
smalley2 = zeros(num_rows*num_runs,1);
spot = 0;
for row = 1:num_rows
    for run = 1:num_runs
        num_carriers = 0;
        noise_floor = 0;
        count = 0;
%         for loc = 19:2:110
%             noise_floor = noise_floor + tx2smalley(row,run,loc+1);
%             count = count+1;
%         end
%         noise_floor = noise_floor/count;
%         for loc = 19:2:110
%             if noise_floor == 0
%                 num_carriers = nan;
%             else
%                 dif = tx2smalley(row,run,loc) - noise_floor;
%                 if(dif > tx2threshold)
%                     num_carriers = num_carriers + 1;
%                 end
%             end
%         end
        for loc = 1:2:127
            if tx2smalley(row,run,loc) - tx2smalley(row,run,loc+1) > tx2threshold
                num_carriers = num_carriers + 1;
            end
        end
        spot = spot + 1;
        smalley2(spot,1) = num_carriers;
    end
end
figure(2);
histogram(cam2)
title('Camacho');
figure(1);
histogram(harrison2)
title('Harrison');
figure(3);
histogram(conference2)
title('Conference');
figure(4);
histogram(smalley2)
title('Smalley');
