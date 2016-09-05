import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.FlowLayout;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.color.ColorSpace;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.geom.AffineTransform;
import java.awt.image.AffineTransformOp;
import java.awt.image.BufferedImage;
import java.awt.image.ColorConvertOp;
import java.awt.image.DataBuffer;
import java.awt.image.IndexColorModel;
import java.io.File;
import java.io.IOException;
import java.net.URL;

import javax.imageio.ImageIO;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.SwingConstants;
import javax.swing.UIManager;



public class Image2Code {
	
	public Image2Code() {
        EventQueue.invokeLater(new Runnable() {
            @Override
            public void run() {
                try {
                    UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
                } catch (Exception ex) {
                	ex.printStackTrace();
                }

                JFrame frame = new JFrame("Image to lcd bitmap converter");
                frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
                frame.setLayout(new GridBagLayout());
                
                GridBagConstraints gbc = new GridBagConstraints();
                gbc.gridx = 0;
                gbc.gridy = 0;
                gbc.weightx = 1;
                gbc.weighty = 1;
                gbc.fill = GridBagConstraints.BOTH;
                frame.add(new ThePanel(frame),gbc);
                
                frame.pack();
                frame.setLocationRelativeTo(null);
                frame.setVisible(true);
            }
        });
    }

	
	public class ThePanel extends JPanel {
	
		
        /**
		 * 
		 */
		private static final long serialVersionUID = 1L; // to remove the warning
		
		@Override
		public Dimension getPreferredSize() {
			return dimension;
		}
		public Dimension getMaximumSize() {
			return dimension;
		}
		private Dimension dimension = new Dimension(1024,768);
		private final JFrame jf;
		private BufferedImage master;
        private BufferedImage blackWhite;
        JLabel fromImg,bwImg;
        private JTextArea codeArea = new JTextArea();
        
        private float scaleFactor = 5.0f;
        private int maxImgWidth = 500, maxImgHeight = 300;
        
        
        private BufferedImage scale(BufferedImage in) {
        	BufferedImage out=null;
        	float actualScaleFactor = Math.min(scaleFactor,Math.min(1.0f*maxImgWidth/in.getWidth(), 1.0f*maxImgHeight/in.getHeight()));
        	if(in.getColorModel() instanceof IndexColorModel) {
        		out = new BufferedImage(Math.round(actualScaleFactor*in.getWidth()),Math.round(actualScaleFactor*in.getHeight()),in.getType(),(IndexColorModel) in.getColorModel());
                		
        	} else {
        		out = new BufferedImage(Math.round(actualScaleFactor*in.getWidth()),Math.round(actualScaleFactor*in.getHeight()),in.getType());
       		
        	}
        	AffineTransformOp scaleOp = new AffineTransformOp(
            		new AffineTransform(actualScaleFactor,0f,0f,actualScaleFactor,0f,0f),AffineTransformOp.TYPE_NEAREST_NEIGHBOR);
        	scaleOp.filter(in,out);
        	return out;
        }
        private String getCodeFromImg(BufferedImage in) {
        	BufferedImage blackWhite = new BufferedImage(in.getWidth(), in.getHeight(), BufferedImage.TYPE_BYTE_BINARY);
    		ColorConvertOp op = new ColorConvertOp(ColorSpace.getInstance(ColorSpace.CS_GRAY), null);
            op.filter(in, blackWhite);

            DataBuffer buf = blackWhite.getRaster().getDataBuffer();
            int byteWidth = (in.getWidth() + 7) /8;
            StringBuffer str = new StringBuffer();
            str.append("{\n");
            int i=0,max=buf.getSize();
            while(i < max) {
            	for(int j=0;j<byteWidth;++j) {
            		str.append("0x"+Integer.toString(buf.getElem(i++),16)+(i != max ?",":""));
            	}
            	str.append("\n");
    		}
            str.append("};\n");
            return str.toString();
        }
        
        void loadURL(URL in) {
        	try {
				master = ImageIO.read(in);
			} catch (IOException e) {
				e.printStackTrace();
			}
			loadImage();
        }
        
        void loadFile(File in) {
        	try {
				master = ImageIO.read(in);
			} catch (IOException e) {
				e.printStackTrace();
			}
			loadImage();
        }
        void loadImage() {
            fromImg.setIcon(new ImageIcon( scale(master) ));
            blackWhite = new BufferedImage(master.getWidth(), master.getHeight(), BufferedImage.TYPE_BYTE_BINARY);
            ColorConvertOp op = new ColorConvertOp(ColorSpace.getInstance(ColorSpace.CS_GRAY), null);
            op.filter(master, blackWhite);
            bwImg.setIcon(new ImageIcon( scale(blackWhite) ));
            codeArea.setText(getCodeFromImg(master));
            
            jf.pack();
        }
        public ThePanel(JFrame jf) {
        	this.jf=jf;
        	setLayout(new GridBagLayout());
        	
        	JButton chooser = new JButton("Choose file");
        	chooser.setAlignmentX(CENTER_ALIGNMENT);
        	add(chooser,new GridBagConstraints(0,0,3,1,0.0,0.0,GridBagConstraints.BASELINE,GridBagConstraints.NONE,new Insets(0,0, 0, 0),0,0));
        	final JFileChooser jfc = new JFileChooser();
        	chooser.addActionListener(new ActionListener() { 
        		@Override
        		public void actionPerformed(ActionEvent e) {
        			switch(jfc.showOpenDialog(ThePanel.this)) {
        			case JFileChooser.APPROVE_OPTION:
        				loadFile(jfc.getSelectedFile());
        				break;
        			default:
        				// TBD display error
        			}
        		}
        	});

        	//JPanel center = new JPanel(new FlowLayout(FlowLayout.CENTER));
        	JPanel south = new JPanel(new FlowLayout(FlowLayout.CENTER));
        	

        	fromImg = new JLabel("Original image:",SwingConstants.LEADING);
        	fromImg.setHorizontalTextPosition(SwingConstants.LEADING);
        	//center.add(fromImg);

        	bwImg = new JLabel("binary image:", SwingConstants.LEADING);
        	bwImg.setHorizontalTextPosition(SwingConstants.LEADING);
        	//center.add(bwImg);

        	add(fromImg,new GridBagConstraints(0,1,2,1,1.0,0.0,GridBagConstraints.BASELINE,GridBagConstraints.HORIZONTAL,new Insets(5,0, 5, 5),0,0));
        	add(bwImg,new GridBagConstraints(2,1,1,1,1.0,0.0,GridBagConstraints.BASELINE,GridBagConstraints.HORIZONTAL,new Insets(5,0, 5, 5),0,0));

        	south.add(new JLabel("Generated code:"));
        	codeArea.setEditable(false);
        	JScrollPane sp = new JScrollPane(codeArea,JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED,JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        	//south.add(sp);
        	add(new JLabel("Generated code:"),new GridBagConstraints(0,2,1,1,0.0,0.0,GridBagConstraints.CENTER,GridBagConstraints.NONE,new Insets(0,0, 0, 0),0,0));
        	add(sp,new GridBagConstraints(1,2,2,1,1.0,1.0,GridBagConstraints.CENTER,GridBagConstraints.BOTH,new Insets(0,0, 0, 0),0,0));
        	
        	//load default image
        	URL defaultImgUrl = this.getClass().getClassLoader().getResource("Adafruit.png");
        	loadURL(defaultImgUrl);
        }
	}
	/**
	 * @param args
	 */
	public static void main(String[] args) {
        new Image2Code();
	}
	
	


}
