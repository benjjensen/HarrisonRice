clear;
close all;

genarrays = false;
plotarrays = true;
includetx1 = false;

if (genarrays)
    load('tx1_linear_signal.mat');
    load('tx1_linear_noisefloor.mat');
    load('tx2_linear_signal.mat');
    load('tx2_linear_noisefloor.mat');
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
    for y = 1:90
        for z = 1:345
            tx1_capacity(y,z) = sum(tx1_cpcty(:,y,z));
            tx2_capacity(y,z) = sum(tx2_cpcty(:,y,z));
        end
    end
    save('tx1_capacity.mat','tx1_capacity');
    save('tx2_capacity.mat','tx2_capacity');
    
    for x = 1:90
        for y = 1:345
            tx2_secrecy_capacity(x,y) = harrison_max_capacity - tx2_capacity(x,y);
            if (tx2_secrecy_capacity(x,y) < 0)
                tx2_secrecy_capacity(x,y) = 0;
            end
        end
    end
    save('tx2_secrecy_capacity.mat','tx2_secrecy_capacity');
end


if (plotarrays)
    load('tx1_capacity.mat');
    load('tx2_capacity.mat');
    load('tx2_secrecy_capacity.mat');
    
    I = imread('ClydeGIMP.png');
    
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
    hm = imagesc(tx2_capacity);
    % title({'Channel Capacity'});
    set(hm,'AlphaData',~isnan(tx2_capacity));
    q = colorbar;
    colormap(jet);
    ylabel(q, 'bits per channel use');
    hm.XData = [36; 380];
    hm.YData = [49; 139];
    hold off
    
    %%%%%%%%%%%%%%%% Secrecy Capacity
    figure();
    imshow(I);
    hold on
    % text(114, 172, '*', 'Color', 'red', 'FontSize', 16);
    text(142, 157, 'tx', 'Color', 'black', 'FontSize', 8);
    hm = imagesc(tx2_secrecy_capacity);
    % title({'Channel Secrecy Capacity'});
    set(hm,'AlphaData',~isnan(tx2_secrecy_capacity));
    q = colorbar;
    colormap(jet);
    ylabel(q, 'bits per channel use');
    hm.XData = [36; 380];
    hm.YData = [49; 139];
    hold off
end