% clear;
close all;

genarrays = false;
plotarrays = true;
includetx1 = false;
findmin = false;

if (genarrays)
    load('tx1_linear_signal.mat');
    load('tx1_linear_noisefloor.mat');
    load('tx2_linear_signal.mat');
    load('tx2_linear_noisefloor.mat');
    load('tx2_snr_db.mat');
    load('harrison_max_capacity.mat');
    
    for x = 1:64
        for y = 1:90
            for z = 1:345
                tx1_noise(x,y,z) = sum(tx1_linear_noisefloor(:,y,z));
                tx2_noise(x,y,z) = sum(tx2_linear_noisefloor(:,y,z));
            end
        end
    end
    
    tx1_noise = tx1_noise / 64;
    tx2_noise = tx2_noise / 64;
    tx1_cpcty = (1/2) * log2(1 + (tx1_linear_signal ./ tx1_noise));
    tx2_cpcty = (1/2) * log2(1 + (tx2_linear_signal ./ tx2_noise));
    tx2_adjusted = (1/2) * log2(1 + (10.^(tx2_snr_db/10)));
    for y = 1:90
        for z = 1:345
            tx1_capacity(y,z) = sum(tx1_cpcty(:,y,z));
            tx2_capacity(y,z) = sum(tx2_cpcty(:,y,z));
            tx2_adjusted_capacity(y,z) = sum(tx2_adjusted(:,y,z));
        end
    end
    save('tx1_capacity.mat','tx1_capacity');
    save('tx2_capacity.mat','tx2_capacity');
    save('tx2_adjusted_capacity.mat','tx2_adjusted_capacity');
    
    for x = 1:90
        for y = 1:345
            tx2_secrecy_capacity(x,y) = harrison_max_capacity - tx2_capacity(x,y);
            tx2_adjusted_secrecy_capacity(x,y) = tx2_adjusted_harrison_max - tx2_adjusted_capacity(x,y);
            if (tx2_secrecy_capacity(x,y) < 0)
                tx2_secrecy_capacity(x,y) = 0;
            end
            if (tx2_adjusted_secrecy_capacity(x,y) < 0)
                tx2_adjusted_secrecy_capacity(x,y) = 0;
            end
        end
    end
    save('tx2_secrecy_capacity.mat','tx2_secrecy_capacity');
    save('tx2_adjusted_secrecy_capacity.mat','tx2_adjusted_secrecy_capacity');
end


if (plotarrays)
    load('tx1_capacity.mat');
    load('tx2_capacity.mat');
    load('tx2_adjusted_capacity.mat');
    load('tx2_secrecy_capacity.mat');
    load('tx2_adjusted_secrecy_capacity.mat');
    load tx2_temp.mat;
    I = imread('ClydeGIMPnoRX.png');
    
    if (includetx1)
        figure();
        imshow(I);
        hold on
%         text(99, 323, '*', 'Color', 'red', 'FontSize', 16);
        text(142, 157, 'tx', 'Color', 'black', 'FontSize', 8);
        hm = imagesc(tx1_capacity);
%         title({'Channel Capacity'});
        set(hm,'AlphaData',~isnan(tx1_capacity));
        q = colorbar;
        colormap(jet);
        ylabel(q, 'bits per channel use');
        hm.XData = [36; 380];
        hm.YData = [49; 139];
        hold off
    end
    
    %%%%%%%%%%%%%%%% Capacity
    figure();
    imshow(I);
    hold on
    % text(114, 172, '*', 'Color', 'red', 'FontSize', 16);
    text(142, 157, 'tx', 'Color', 'black', 'FontSize', 8);
    hm = imagesc(tx2_adjusted_capacity);
    % title({'Channel Capacity'});
    set(hm,'AlphaData',~isnan(tx2_adjusted_capacity));
    q = colorbar;
    q.Position = [.855 .2695 .016 .566];
    colormap(jet);
    ylabel(q, 'bits per channel use');
    hm.XData = [36; 380];
    hm.YData = [49; 139];
    hold off
    
    %%%%%%%%%%%%%%%% Secrecy Capacity
    I = imread('ClydeGIMPnoRX.png');
    figure();
    imshow(I);
    hold on
    % text(114, 172, '*', 'Color', 'red', 'FontSize', 16);
    text(142, 157, 'tx', 'Color', 'black', 'FontSize', 8);
    %     hm = imagesc(tx2_secrecy_capacity);
%     hm = imagesc(tx2_temp);
    hm = imagesc(tx2_adjusted_secrecy_capacity);
    % title({'Channel Secrecy Capacity'});
    %     set(hm,'AlphaData',~isnan(tx2_secrecy_capacity));
%     set(hm,'AlphaData',~isnan(tx2_temp));
    set(hm,'AlphaData',~isnan(tx2_adjusted_secrecy_capacity));
%     set(hm,'AlphaData',tx2_temp(41,77));
%%%%% Harrison's best location is at (65,92)-> 55.8995, with second best at (39,76)->55.8132 in the array
% Conference Room min at (64,45) -> 40.1274
% Smalley min at (65,148) -> 8.0705
% Camacho min at (65,154) -> 19.354
% Chambers min at (65,219) -> 55.3335
%     text(105, 73, 'rx', 'Color', 'black', 'FontSize', 8);
    q = colorbar;
    q.Position = [.855 .2695 .016 .566];
    colormap(jet);
    ylabel(q, 'bits per channel use');
    hm.XData = [36; 380];
    hm.YData = [49; 139];
    hold off
    

end

if(findmin)
    min = 200;
    for x = 34:65
        for y = 213:240
            if(isnan(tx2_adjusted_secrecy_capacity(x,y)))
            elseif(tx2_adjusted_secrecy_capacity(x,y) < min)
                disp("at (" + string(x) + "," + string(y) + ") value is " + string(tx2_secrecy_capacity(x,y)));
                min = tx2_adjusted_secrecy_capacity(x,y);
            end
        end
    end
end