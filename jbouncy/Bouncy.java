import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.util.Observable;
import java.util.Observer;
import java.util.Timer;
import java.util.TimerTask;
import java.util.Scanner;
import java.util.Vector;
import javax.swing.*;
import javax.swing.SwingUtilities;
import javax.swing.filechooser.*;
import javax.swing.JOptionPane.*;

//---------------------------------------------------------------------

public class Bouncy extends JFrame {

    public static final int DEFAULT_SIZE = 500;

    JPanel  buttonPanel = new JPanel();
    JButton openButton  = new JButton("Open");
    JButton runButton   = new JButton("Run");
    JButton stepButton  = new JButton("Step");
    JButton stopButton  = new JButton("Stop");
    JFileChooser fileChooser = new JFileChooser();
    Timer timer;
    Balls balls = new Balls();
    View view = new View(balls);

    public static void main(String[] args) {
        Bouncy c = new Bouncy();
        if (args.length > 0)
            c.init(args[0]);
        else
            c.init(null);
        c.setSize(DEFAULT_SIZE, DEFAULT_SIZE);
        c.setVisible(true);
        c.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        c.balls.setLimits(c.view.getWidth(), c.view.getHeight());
        if (args.length > 0)
            c.setTitle("0/" + c.balls.getNumFrames());
        c.balls.notifyObservers();
    }
    
    public void init(String fname) {
        boolean hasData = false;
        layOutComponents();
        if (fname != null) {
            hasData = balls.setData(new File(fname));
            setRunnable();
        }
        attachListenersToComponents();
        fileChooser.setCurrentDirectory(new File("."));
        balls.addObserver(view);
        balls.setLimits(view.getWidth(), view.getHeight());
    }
    
    private void layOutComponents() {
        setLayout(new BorderLayout());
        this.add(BorderLayout.SOUTH, buttonPanel);
        buttonPanel.add(openButton);
        buttonPanel.add(runButton);
        buttonPanel.add(stepButton);
        buttonPanel.add(stopButton);
        setNoData();
        this.add(BorderLayout.CENTER, view);
    }

    private void setNoData() {
        runButton.setEnabled(false);
        stepButton.setEnabled(false);
        stopButton.setEnabled(false);        
    }

    private void setRunnable() {
        runButton.setEnabled(true);
        stepButton.setEnabled(true);
        stopButton.setEnabled(false);
    }

    private void setStoppable() {
        runButton.setEnabled(false);
        stepButton.setEnabled(false);
        stopButton.setEnabled(true);
    }

    private void attachListenersToComponents() {
        runButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                setStoppable();
                timer = new Timer(true);
                timer.schedule(new Strobe(), 0, 40); // 25 times a second
            }
        });
        stepButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                setRunnable();
                balls.setLimits(view.getWidth(), view.getHeight());
                Bouncy.this.setTitle(balls.makeOneStep() + "/" +
                                     balls.getNumFrames());
                
            }
        });
        stopButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                setRunnable();
                timer.cancel();
            }
        });
        openButton.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                setNoData();
                if (timer != null)
                    timer.cancel();
                int returnVal = fileChooser.showOpenDialog(Bouncy.this);
                if (returnVal == JFileChooser.APPROVE_OPTION) {
                    if (balls.setData(fileChooser.getSelectedFile())) {
                        balls.setLimits(view.getWidth(), view.getHeight());
                        balls.notifyObservers();
                        setRunnable();
                    }
                } else {
                    setRunnable();
                }
            }
        });
    }

    private class Strobe extends TimerTask { // An inner class
        public void run() {
            balls.setLimits(view.getWidth(), view.getHeight());
            Bouncy.this.setTitle(balls.makeOneStep() + "/" +
                                 balls.getNumFrames());
        }
    }
}

//---------------------------------------------------------------------

class Frame {
    private float xPositions[];
    private float yPositions[];
    private int   color[];

    public float getX(int i) { return xPositions[i]; }
    public float getY(int i) { return yPositions[i]; }
    public int   getColor(int i) { return color[i]; }

    public Frame readTextFrame(Scanner s, int numBalls) 
        throws IOException, java.util.InputMismatchException {
        xPositions = new float[numBalls];
        yPositions = new float[numBalls];
        color      = new int  [numBalls];
        for (int i = 0; i < numBalls; ++i) {
            xPositions[i] = s.nextFloat();
            yPositions[i] = s.nextFloat();
            color[i]      = s.nextInt();
        }
        return this;
    }

    public Frame readBinFrame(DataInputStream in, int numBalls) 
        throws IOException, EOFException {
        xPositions = new float[numBalls];
        yPositions = new float[numBalls];
        color      = new int  [numBalls];
        for (int i = 0; i < numBalls; ++i) {
            xPositions[i] = in.readFloat();
            yPositions[i] = in.readFloat();
            color[i]      = in.readInt();
        }
        return this;
    }
}

//---------------------------------------------------------------------

class Balls extends Observable {
    public final int BALL_SIZE = 5;

    private Vector frames = new Vector();
    private Frame currentFrame;
    private float scale;
    private int xLimit = Bouncy.DEFAULT_SIZE-BALL_SIZE;
    private int yLimit = Bouncy.DEFAULT_SIZE-BALL_SIZE;
    private int numBalls;
    private int numFrames;
    private int currentFrameIndex;

    public boolean setData(File file) {
        Scanner s = null;
        boolean status = false;
        try {
            s = new Scanner(new BufferedReader(new FileReader(file)));
            String tag = s.next();
            if (tag.equals("SPHView00")) {
                readTextData(s);
                s.close();
            } else if (tag.equals("SPHView01")) {
                s.close();
                DataInputStream in = new DataInputStream(
                    new BufferedInputStream(new FileInputStream(file)));
                readBinData(in);
                in.close();
            }  else {
                System.out.println("Unknown tag");
            }
            setChanged();
            status = true;
        } catch(IOException e) {
            System.out.println("Failure during read");
        } catch (java.util.InputMismatchException e) {
            System.out.println("Malformed text input file");
        } finally {
            s.close();
        }
        return status;
    }

    public void setLimits(int xLimit, int yLimit) {
        this.xLimit = xLimit - BALL_SIZE;
        this.yLimit = yLimit - BALL_SIZE;
    }

    public int getNumBalls() {
        return numBalls;
    }

    public int getNumFrames() {
        return frames.size();
    }

    public int getX(int i) {
        return (int) (currentFrame.getX(i)/scale * xLimit);
    }

    public int getY(int i) {
        return (int) ((1-currentFrame.getY(i)/scale) * yLimit);
    }

    public Color getColor(int i) {
        if (currentFrame.getColor(i) == 0) {
            return Color.red;
        } else {
            return Color.blue;
        }
    }

    public int makeOneStep() {
        // Do the work
        ++currentFrameIndex;
        if (currentFrameIndex >= getNumFrames())
            currentFrameIndex = 0;
        currentFrame = (Frame) frames.get(currentFrameIndex);

        // Notify observers
        setChanged();
        notifyObservers();
        return currentFrameIndex;
    }

    private void readTextData(Scanner s) 
        throws IOException, java.util.InputMismatchException {
        frames.clear();
        currentFrameIndex = 0;
        numBalls = s.nextInt();
        scale = s.nextFloat();
        while (s.hasNextFloat()) {
            frames.add(new Frame().readTextFrame(s, numBalls));
        }
        if (s.hasNext())
            System.out.println("Quit at: '" + s.next() + "'");
        currentFrame = (Frame) frames.get(currentFrameIndex);
    }

    private void readBinData(DataInputStream in) 
        throws IOException {
        frames.clear();
        currentFrameIndex = 0;
        in.readLine();
        numBalls = in.readInt();
        scale = in.readFloat();
        try {
            while (true) 
                frames.add(new Frame().readBinFrame(in, numBalls));
        } catch (EOFException e) {
        }
        currentFrame = (Frame) frames.get(currentFrameIndex);
    }
}

//---------------------------------------------------------------------

class View extends JPanel implements Observer {
    Balls balls;

    View(Balls balls) {
        this.balls = balls;
    }

    public void paint(Graphics g) {
        g.setColor(Color.WHITE);
        g.fillRect(0, 0, getWidth(), getHeight());
        for (int i = 0; i < balls.getNumBalls(); ++i) {
            g.setColor(balls.getColor(i));
            g.fillOval(balls.getX(i), balls.getY(i),
                       balls.BALL_SIZE, balls.BALL_SIZE);
        }
    }

    public void update(Observable obs, Object arg) {
        repaint();
    }
}

