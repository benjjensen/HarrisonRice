clear; close all;
load SPECIALFOLDER/processed.mat;

figure();
imagesc(harrisonComplete);
ax1 = gca; title('Harrison');
colorbar;
caxis([0 500]);
ax1.YDir = 'normal'; drawnow;

figure();
imagesc(smalleyComplete);
ax2 = gca; title('Smalley');
colorbar;
caxis([0 500]);
ax2.YDir = 'normal'; drawnow;

figure();
imagesc(camachoComplete);
ax3 = gca; title('Camacho');
colorbar;
caxis([0 500]);
ax3.YDir = 'normal'; drawnow;

hmax = max(max(harrisonComplete));
smax = max(max(smalleyComplete));
cmax = max(max(camachoComplete));