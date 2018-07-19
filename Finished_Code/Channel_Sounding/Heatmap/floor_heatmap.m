
close all;

load('camacho.mat');
load('chambers.mat');
load('conference.mat');
load('hall_pwelch.mat');
load('harrison.mat');
load('smalley.mat');
load('tx2camacho.mat');
load('tx2chambers.mat');
load('tx2conference.mat');
load('tx2hallway.mat');
load('tx2harrison.mat');
load('tx2smalley.mat');

array_mesh_all();
pwelch_all_2();
num_rows = 90;
num_runs = 345;
num_total = 400;
Nfft = 2*64;
FF = -0.5:1/Nfft:0.5-1/Nfft;
FF = 20*FF;
HH = zeros(num_runs,1);
Floor = nan(num_rows,num_runs);
Final_floor = nan(num_rows,num_runs);
avg_floor = zeros(num_rows,num_runs);
threshold = 15;
tx2HH = zeros(num_runs,1);
tx2Floor = nan(num_rows,num_runs);
tx2Final_floor = nan(num_rows,num_runs);
tx2avg_floor = zeros(num_rows,num_runs);
tx2threshold = 28;
for row = 1:num_rows
    for runs = 1:num_runs     
        num_carriers = 0;
        noise_floor = 0;
        count = 0;
        tx2num_carriers = 0;
        tx2noise_floor = 0;
        tx2count = 0;
        for loc = 19:2:110
            if xx(row,runs,loc+1) == 0
                noise_floor = 0;
                count = 1;
            else
                noise_floor = noise_floor + xx(row,runs,loc+1);
                count = count+1;
            end
        end
        noise_floor = noise_floor/count;
        
        for loc = 19:2:110
            if tx2xx(row,runs,loc+1) == 0
                tx2noise_floor = 0;
                tx2count = 1;
            else
                tx2noise_floor = tx2noise_floor + tx2xx(row,runs,loc+1);
                tx2count = tx2count+1;
            end
        end
        tx2noise_floor = tx2noise_floor/tx2count;
        
        for loc = 19:2:110
            if noise_floor == 0
                num_carriers = nan;
            else
                dif = xx(row,runs,loc) - noise_floor;
                if(dif > threshold)
                    num_carriers = num_carriers + 1;
                end
            end
        end
        
        for loc = 19:2:110
            if tx2noise_floor == 0
                tx2num_carriers = nan;
            else
                tx2dif = tx2xx(row,runs,loc) - tx2noise_floor;
                if(tx2dif > tx2threshold)
                    tx2num_carriers = tx2num_carriers + 1;
                end
            end
        end
        
%         for loc = 1:2:length(YYplot)
%             dif = hall_pwelch(row,runs,loc) - hall_pwelch(row,runs,loc+1);
%             if(dif > 7)
%                 a = a + 1;
%             end
%         end
        HH(runs) = num_carriers;
        
        tx2HH(runs) = tx2num_carriers;
    end
    Floor(row,1:num_runs) = HH;
    tx2Floor(row,1:num_runs) = tx2HH;
end
% for row = 1:num_rows
%     for runs = 1:num_runs
%         s = "hallway_" + string(row);
%         hh = eval(s);
%         YY = pwelch(hh(:,runs),boxcar(Nfft),0,Nfft,'twosided');
%         YYplot = 10*log10(abs(fftshift(YY)));
%         a = 0;
%         c = 0;
%         noise_floor = 0;
%         i = 0;
%         for loc = 19:2:110
%             c = c + YYplot(loc);
%             noise_floor = noise_floor + YYplot(loc+1);
%             i = i+1;
%         end
%         noise_floor = noise_floor/i;
%         for loc = 19:2:110
%             dif = YYplot(loc) - noise_floor;
%             if(dif > threshold)
%                 a = a + 1;
%             end
%         end
% %         for loc = 1:2:length(YYplot)
% %             dif = YYplot(loc) - YYplot(loc+1);
% %             if(dif > 7)
% %                 a = a + 1;
% %             end
% %         end
%         HH(runs) = a;
%     end
%     Hallway(row,1:num_runs) = HH;
% end
% for x = 1:num_runs
%     for y = 1:num_rows
%         avg = Floor(y,x);
%         num = 1;
%         if x ~= 1
%             avg = avg + Floor(y,x-1);
%             num = num + 1;
%         end
%         if x ~= num_runs
%             avg = avg + Floor(y,x+1);
%             num = num + 1;
%         end
%         if y ~= 1
%             avg = avg + Floor(y-1,x);
%             num = num + 1;
%         end
%         if y ~= num_rows
%             avg = avg + Floor(y+1,x);
%             num = num + 1;
%         end
%         avg_floor(y,x) = avg/num;
%         Final_floor(24-x,y) = avg/num;
% %         Hall(24-x,y) = Hallway(y,x);
%     end
% end
% for x = 1:num_runs
%     for y = 1:num_rows
%         avg = avg_hall(y,x);
%         num = 1;
%         if x ~= 1
%             avg = avg + avg_hall(y,x-1);
%             num = num + 1;
%         end
%         if x ~= num_runs
%             avg = avg + avg_hall(y,x+1);
%             num = num + 1;
%         end
%         if y ~= 1
%             avg = avg + avg_hall(y-1,x);
%             num = num + 1;
%         end
%         if y ~= num_rows
%             avg = avg + avg_hall(y+1,x);
%             num = num + 1;
%         end
%         Hall(24-x,y) = avg/num;
% %         Hall(24-x,y) = Hallway(y,x);
%     end
% end
%%%internet method
% Floor(90,345) = -20;
% [height,width,depth] = size(I);
% OverlayImage=[];
% X(1:331) = Floor(:,1);
% Y(1:508) = Floor(1,:);
% F = scatteredInterpolant(X,Y, Floor(:,:), 'linear');
% for i = 1:height-1
%    for j = 1:width-1
%           OverlayImage(i,j) = F(i,j);
%    end
% end
% alpha = (~isnan(OverlayImage))*0.6;
% imshow(picture);
% hold on
% OverlayImage = imshow( OverlayImage );
% % Set the color limits to be relative to the data values
% caxis auto  
% colormap( OverlayImage.Parent, jet );
% colorbar( OverlayImage.Parent );
% % Set the AlphaData to be the transparency matrix created earlier
% set( OverlayImage, 'AlphaData', alpha );

%%%original method
% figure(1);
% imshow(I);
% figure(2);
% % hold on;
% set(gcf, 'Position', [0 1000 1500 400]);
% heat = heatmap(Floor);
% myColor = colormap(colormap('jet'));
% % myColor = colormap(flipud(colormap('hot')));
% heat.Colormap = myColor;
% heat.MissingDataColor = 'white';
% % alpha(heat,0);
% grid off;
figure(1);
subplot(2,1,1);
I = imread('floor_plan.png');
imshow(I);
hold on;
floormap = imagesc(Floor);
floormap.XData = [13; 315];
floormap.YData = [61; 137];
% myColor = colormap(flipud(colormap('hot')));
% colormap(myColor);
colormap('jet')
floormap.AlphaData = .6;
set(floormap,'AlphaData',~isnan(Floor));
colorbar;
hold off;

subplot(2,1,2);
imshow(I);
hold on;
tx2floormap = imagesc(tx2Floor);
tx2floormap.XData = [13; 315];
tx2floormap.YData = [61; 137];
myColor = colormap(flipud(colormap('hot')));
colormap('jet');
tx2floormap.AlphaData = .6;
set(tx2floormap,'AlphaData',~isnan(tx2Floor));
colorbar;
hold off;