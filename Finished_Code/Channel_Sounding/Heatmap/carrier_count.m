close all;
threshold = 13;
tx2threshold = 26.5;
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

camacho_best_carrier = zeros(1,64);
camacho_best_carrier2 = zeros(1,64);
chambers_best_carrier = zeros(1,64);
chambers_best_carrier2 = zeros(1,64);
conference_best_carrier = zeros(1,64);
conference_best_carrier2 = zeros(1,64);
harrison_best_carrier = zeros(1,64);
harrison_best_carrier2 = zeros(1,64);
smalley_best_carrier = zeros(1,64);
smalley_best_carrier2 = zeros(1,64);

[~,Icam1] = max(cam1);
[~,Icam2] = max(cam2);
[~,Ichamb1] = max(chambers1);
[~,Ichamb2] = max(chambers2);
[~,Iconf1] = max(conference1);
[~,Iconf2] = max(conference2);
[~,Iharr1] = max(harrison1);
[~,Iharr2] = max(harrison2);
[~,Ismall1] = max(smalley1);
[~,Ismall2] = max(smalley2);
[num_rows,num_runs,~] = size(camacho);
row = floor((Icam1-1)/num_runs) + 1;
run = mod((Icam1-1),num_runs) + 1;
for loc = 1:2:127
    if camacho(row,run,loc) - camacho(row,run,loc+1) > threshold
        camacho_best_carrier(1,(loc+1)/2) = 3;
    end
end
[num_rows,num_runs,~] = size(tx2camacho);
row = floor((Icam2-1)/num_runs) + 1;
run = mod((Icam2-1),num_runs) + 1;
for loc = 1:2:127
    if tx2camacho(row,run,loc) - tx2camacho(row,run,loc+1) > tx2threshold
        camacho_best_carrier2(1,(loc+1)/2) = 3;
    end
end
[num_rows,num_runs,~] = size(chambers);
row = floor((Ichamb1-1)/num_runs) + 1;
run = mod((Ichamb1-1),num_runs) + 1;
for loc = 1:2:127
    if chambers(row,run,loc) - chambers(row,run,loc+1) > threshold
        chambers_best_carrier(1,(loc+1)/2) = 2;
    end
end
[num_rows,num_runs,~] = size(tx2chambers);
row = floor((Ichamb2-1)/num_runs) + 1;
run = mod((Ichamb2-1),num_runs) + 1;
for loc = 1:2:127
    if tx2chambers(row,run,loc) - tx2chambers(row,run,loc+1) > tx2threshold
        chambers_best_carrier2(1,(loc+1)/2) = 2;
    end
end
[num_rows,num_runs,~] = size(conference);
row = floor((Iconf1-1)/num_runs) + 1;
run = mod((Iconf1-1),num_runs) + 1;
for loc = 1:2:127
    if conference(row,run,loc) - conference(row,run,loc+1) > threshold
        conference_best_carrier(1,(loc+1)/2) = 1;
    end
end
[num_rows,num_runs,~] = size(tx2conference);
row = floor((Iconf2-1)/num_runs) + 1;
run = mod((Iconf2-1),num_runs) + 1;
for loc = 1:2:127
    if tx2conference(row,run,loc) - tx2conference(row,run,loc+1) > tx2threshold
        conference_best_carrier2(1,(loc+1)/2) = 1;
    end
end
[num_rows,num_runs,~] = size(harrison);
row = floor((Iharr1-1)/num_runs) + 1;
run = mod((Iharr1-1),num_runs) + 1;
for loc = 1:2:127
    if harrison(row,run,loc) - harrison(row,run,loc+1) > threshold
        harrison_best_carrier(1,(loc+1)/2) = 5;
    end
end
[num_rows,num_runs,~] = size(tx2harrison);
row = floor((Iharr2-1)/num_runs) + 1;
run = mod((Iharr2-1),num_runs) + 1;
for loc = 1:2:127
    if tx2harrison(row,run,loc) - tx2harrison(row,run,loc+1) > tx2threshold
        harrison_best_carrier2(1,(loc+1)/2) = 5;
    end
end
[num_rows,num_runs,~] = size(smalley);
row = floor((Ismall1-1)/num_runs) + 1;
run = mod((Ismall1-1),num_runs) + 1;
for loc = 1:2:127
    if smalley(row,run,loc) - smalley(row,run,loc+1) > threshold
        smalley_best_carrier(1,(loc+1)/2) = 4;
    end
end
[num_rows,num_runs,~] = size(tx2smalley);
row = floor((Ismall2-1)/num_runs) + 1;
run = mod((Ismall2-1),num_runs) + 1;
for loc = 1:2:127
    if tx2smalley(row,run,loc) - tx2smalley(row,run,loc+1) > tx2threshold
        smalley_best_carrier2(1,(loc+1)/2) = 4;
    end
end

figure(5);
hold on;
title(['\fontsize{12} Tx_2 @' num2str(tx2threshold) ' dB']);
bar(harrison_best_carrier2,'DisplayName',['Harrison - ' num2str(sum(harrison_best_carrier2)/5)]);
bar(smalley_best_carrier2,'DisplayName',['Smalley - ' num2str(sum(smalley_best_carrier2)/4)]);
bar(chambers_best_carrier2,'DisplayName',['Chambers - ' num2str(sum(chambers_best_carrier2)/2)]);
bar(camacho_best_carrier2,'DisplayName',['Camacho - ' num2str(sum(camacho_best_carrier2)/3)]);
bar(conference_best_carrier2,'DisplayName',['Conference - ' num2str(sum(conference_best_carrier2))]);
legend;
hold off