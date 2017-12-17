#include "Bird.h"
#include "CCanvas.h"

/*
 * Initialise buffers to draw
 */
void Bird::init() {
    body.init();
    head.init();
    tail.init();
    left_wing_far.init();
    left_wing_close.init();
    right_wing_far.init();
    right_wing_close.init();
    texture.setTexture();

    psi = 0.0f;
    direction = startDirection;
    position = flyPath(0.0f);
}

/*
 * Draw all parts
 * Each part is saved as an object at the origin (in general,
 * one side at the origin for all pieces except the body).
 *
 * Before drawing, translate each object back into the correct
 * position so that the bird is complete. These transformations are
 * hardcoded based on how the files were created, the transformations
 * are detailed in the file part_translations.txt
 * Do the transformations in the same order as they are described in
 * the file, but reverse the sign (except for the depth axis that is
 * inverted in Blender) and remember that Z and Y are inverted in Blender.
 *
 * Finally apply local transformations to animate the bird.
 */
void Bird::draw() {

    /*
     * transformations to see bird from different perspectives
     */
    //    GLfloat scale = 0.2f;
    //    glRotatef(45.0f, 0.0f, 0.0f, 1.0f);

    // oscillate
    //    glRotatef(100*tau, 0.0f, 1.0f, 0.0f);
    //    glRotatef(-45.0f, 0.0f, 1.0f, 0.0f);
    //    glRotatef(50*tau, 1.0f, 0.0f, 0.0f);
    //    glTranslatef(4.0f, 2.0f, 0.0f);
    //    glScalef(scale, scale, scale);

    /*
     * Slight correction to whole bird (it should not look too much down)
     */
    glRotatef(-6.0f, 1, 0, 0);

    /* Body:
     * - static
     * - center at origin
     * - no movement applied
     */
    glPushMatrix(); // push body
    if (this->animate) {
        glTranslatef(0.0f, -0.1 * sin(psi), 0.0f);
        glRotatef(sin(psi), 1.0f, 0.0f, 0.0f);
    }
    body.draw();

    /* Head:
     * - can move (e.g. look a bit around)
     * - back of head (neck connection) at origin
     * - translated forward and down
     */
    glPushMatrix();
    glTranslatef(0.0f, -0.5f, 1.2f);
    head.draw();
    glPopMatrix();

    /* Tail:
     * - can move (e.g. up and down)
     * - connection with body at origin
     * - translated backwards
     */
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -1.75f);
    tail.draw();
    glPopMatrix();

    /* Left wing:
     * - can move (e.g. up and down)
     * - connection with body at origin
     * - two pieces, many transformations (also nested)
     */
    glPushMatrix(); // push left_wing_close
    glTranslatef(0.7f, 0.2f, 0.0f);
    glRotatef(16.0f, 0.0f, 0.0f, 1.0f);  // Z: + 16.0
    glRotatef(-6.75f, 0.0f, 1.0f, 0.0f); // Y: -  6.75

    // animation
    if (this->animate) {
        glRotatef(32 * sin(psi), 0.0f, 0.0f, 1.0f);
    }
    left_wing_close.draw();

    /* translate and draw far part after close part of wing
     */
    glPushMatrix(); // push left_wing_far
    glTranslatef(1.6f, 0.0f, 1.0f);
    glRotatef(-12.0f, 0.0f, 0.0f, 1.0f); // Z: - 12.0

    // animation
    if (this->animate) {
        glRotatef(16 * sin(psi), 0.0f, 0.0f, 1.0f);
    }
    left_wing_far.draw();
    glPopMatrix(); // pop left_wing_far

    glPopMatrix(); // pop left_wing_close

    /* Right wing:
     * - can move (e.g. up and down)
     * - connection with body at origin
     * - two pieces, many transformations (also nested)
     */
    glPushMatrix(); // push right_wing_close
    glTranslatef(-0.7f, 0.2f, 0.0f);
    glRotatef(-16.0f, 0.0f, 0.0f, 1.0f); // Z: - 16.0
    glRotatef(6.75f, 0.0f, 1.0f, 0.0f);  // Y: +  6.75

    // animation
    if (this->animate) {
        glRotatef(32 * sin(psi), 0.0f, 0.0f, -1.0f);
    }
    right_wing_close.draw();

    /* translate and draw far part after close part of wing
     */
    glPushMatrix(); // push right_wing_far
    glTranslatef(-1.25f, 0.0f, 0.5f);
    glRotatef(12.0f, 0.0f, 0.0f, 1.0f); // Z: + 12.0

    // animation
    if (this->animate) {
        glRotatef(16 * sin(psi), 0.0f, 0.0f, -1.0f);
    }
    right_wing_far.draw();
    glPopMatrix(); // pop right_wing_far

    glPopMatrix(); // pop right_wing_close

    glPopMatrix(); // pop body
}

/*
 * Increment number for animations
 */
void Bird::inc() {
    psi += psiIncrement;
}

void Bird::fly(GLfloat tau) {
    if (this->move) {
        Point3d currPos = this->position;
        Point3d nextPos = flyPath(tau);

        glTranslatef(currPos.x(), currPos.y(), currPos.z());

        GLfloat scale = 0.2f;
        glScalef(scale, scale, scale);

        direction = (nextPos - currPos).normalized();

        Point3d directionXZ = Point3d(direction.x(), 0.0f, direction.z());
        Point3d startDirectionXZ = Point3d(startDirection.x(), 0.0f, startDirection.z());
        GLfloat yAngle = startDirectionXZ.getAngle(directionXZ) * 180 / PI;

        GLfloat sign = (startDirectionXZ ^ directionXZ).y();
        yAngle = copysign(yAngle, sign);
        glRotatef(yAngle, 0.0f, 1.0f, 0.0f);

        // save position
        this->position = nextPos;
    }
}

int Bird::orientationTest(Point3d a, Point3d mid, Point3d b) {
    // orientation test on x and z axys
    // 0: collinear
    // positive: left
    // negative: right
    double cross = (mid.x() - a.x()) * (b.z() - a.z()) - (mid.z() - a.z()) * (b.x() - a.x());
    if(cross < -0.0001) return -1; // right
    if(cross > 0.0001) return 1;
    return 0;                      // collinear
}

//void Bird::test() {
//    float xcr, ycr;   //Points on the Catmull-Rom spline
//    float dx, dy;     //tangent components
//    glClear(GL_COLOR_BUFFER_BIT);
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();

//    glPointSize(6.0);
//    glColor3f(1.0, 0.0, 1.0);
//    glBegin(GL_POINTS);
//    for(int i = 0; i < numPts; i++)
//           glVertex2f(x[i], y[i]);
//    glEnd();

//    if(numPts > 3)
//    {
//       glColor3f(1.,0.,0.);
//       glBegin(GL_LINES); //draw tangents
//       for(int i = 1; i < numPts-1; i++){
//           dx = 0.2*(x[i+1]-x[i-1]);
//           dy = 0.2*(y[i+1]-y[i-1]);
//           glVertex2f(x[i]-dx, y[i]-dy);
//           glVertex2f(x[i]+dx,y[i]+dy);
//        }
//        glEnd();

//        glColor3f(0., 0., 1.);
//        glBegin(GL_LINE_STRIP);
//        for(int i = 1; i < numPts-2; i++)
//        {
//            for(int k = 0;  k < 50; k++){    //50 points
//               float t = k*0.02;  //Interpolation parameter
//               //--Eq. (7.34)--
//               xcr = x[i] + 0.5*t*(-x[i-1]+x[i+1])
//                   + t*t*(x[i-1] - 2.5*x[i] + 2*x[i+1] - 0.5*x[i+2])
//                   + t*t*t*(-0.5*x[i-1] + 1.5*x[i] - 1.5*x[i+1] + 0.5*x[i+2]);
//               ycr = y[i] + 0.5*t*(-y[i-1]+y[i+1])
//                   + t*t*(y[i-1] - 2.5*y[i] + 2*y[i+1] - 0.5*y[i+2])
//                   + t*t*t*(-0.5*y[i-1] + 1.5*y[i] - 1.5*y[i+1] + 0.5*y[i+2]);
//               glVertex2f(xcr, ycr);
//           }
//        }
//        glEnd();
//    }
//    glFlush();
//}

Point3d Bird::flyPath(GLfloat tau) {
    float xcr, zcr;   //Points on the Catmull-Rom spline

    if(pathT == 0) indexPath = (indexPath + 1) % maxPath;

    int prev = (indexPath == 0) ? maxPath - 1 : indexPath - 1;
    Point3d p1 = path[prev];
    Point3d p2 = path[indexPath];
    Point3d p3 = path[(indexPath + 1) % maxPath];
    Point3d p4 = path[(indexPath + 2) % maxPath];

    if (pathT == 0) {
       float distance = (p3 - p2).norm();
       speed = ceil(distance * 15);
       cout << speed << ' ' << distance << endl;
    }

    float t = (float)pathT / (float)speed;  //Interpolation parameter

    xcr = p2.x() + 0.5*t*(-p1.x()+p3.x())
            + t*t*(p1.x() - 2.5*p2.x() + 2*p3.x() - 0.5*p4.x())
            + t*t*t*(-0.5*p1.x() + 1.5*p2.x() - 1.5*p3.x() + 0.5*p4.x());
    zcr = p2.z() + 0.5*t*(-p1.z()+p3.z())
            + t*t*(p1.z() - 2.5*p2.z() + 2*p3.z() - 0.5*p4.z())
            + t*t*t*(-0.5*p1.z() + 1.5*p2.z() - 1.5*p3.z() + 0.5*p4.z());


    pathT = (pathT + 1) % speed;
    return Point3d(xcr, 0, zcr);
}

/*
 * Return position relative to the given tau
 */
//Point3d Bird::flyPath(GLfloat tau) {


//    // initialization position
//    if (tau == 0.0f) {
//        return path[indexPath++];
//    }

//    Point3d nextPath = path[indexPath];

//    Point3d nextPosition;

//    if (forwarding) {
//        // orientation test
//        double orientation = orientationTest(position, position + direction, nextPath);

//        if (orientation == 0) {
//            // go straight

//            // calculate next point
//            nextPosition += position + direction * speed;

//            if (((nextPath.x() <= nextPosition.x() && nextPath.x() >= position.x()) || (nextPath.x() >= nextPosition.x() && nextPath.x() <= position.x())) &&
//                ((nextPath.z() <= nextPosition.z() && nextPath.z() >= position.z()) || (nextPath.z() >= nextPosition.z() && nextPath.z() <= position.z()))) {
//                // check if next point overtake the nextPath
//                // if so, select a new path
//                indexPath = (indexPath + 1) % maxPath;
//            }

//        } else {
//            // the points are not colinear
//            forwarding = false;
//            // may add random radius
//            radius = 4;
//        }
//    }

//    if (!forwarding) {
//        // turn
//        direction = (nextPath - position).normalized();
//        nextPosition += position + direction * speed;
//        forwarding = true;
//    }

//    // remember to increment indexPath!!!!!

//    return nextPosition;
//}
