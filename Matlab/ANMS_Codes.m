%% read and display test image
pathToImage = '../Images/test.png'; % specify path to the test image
I = imread(pathToImage);
figure('Name','Original Image'); hold on;
imshow(I);

%% detect and display FAST features
corners = detectFASTFeatures(I, 'MinQuality',0.0001, 'MinContrast', 0.01);
figure('Name', 'Detected FAST features');
imshow(I); hold on;
plot(corners);

%% apply SSC algorithm
numRetPoints = 750; % number of points you need
tolerance = 0.1; % tolerance of the number of return points
rows = size(I,1);
cols = size(I,2);
[~, ind] = sort(corners.Metric,'descend');
cornersSorted = corners(ind);

% get homegenously distributed keypoints
selectedIdx = SSC(double(cornersSorted.Location), numRetPoints, tolerance, cols, rows);
selectedCorners = cornersSorted((selectedIdx+1)'); % +1 since matlab is one-indexed

%% visualizing results
figure('Name', 'Selected Keypoints');
imshow(I); hold on;
plot(selectedCorners);