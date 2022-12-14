/**
 * @file geometry_algo.hpp
 * @author Radica
 * @brief
 * @version 0.1
 * @date 2022-11-25
 *
 * @copyright Copyright (c) 2022
 *
 */
#ifndef REDEDA_GEOMETRY_ALGO_H
#define REDEDA_GEOMETRY_ALGO_H

#include <cmath>

namespace geometry
{

    /* 常用的常量定义 */
    const double INF = 1E200;
    const double EP = 1E-10;
    const int MAXV = 300;
    const double PAI = 3.14159265;

    /*************************基本几何结构***************************/

    /**
     * @brief 点
     *
     */
    struct POINT
    {
        double x;
        double y;
        POINT(double a = 0, double b = 0)
        {
            x = a;
            y = b;
        } // constructor
    };

    /**
     * @brief 线段
     *
     */
    struct LINESEG
    {
        POINT s;
        POINT e;
        LINESEG(POINT a, POINT b)
        {
            s = a;
            e = b;
        }
        LINESEG() {}
    };

    /**
     * @brief 直线
     *
     */
    struct LINE
    {
        double a;
        double b;
        double c;
        LINE(double d1 = 1, double d2 = -1, double d3 = 0)
        {
            a = d1;
            b = d2;
            c = d3;
        }
    };

    /**
     * @brief 方向
     *
     */
    enum class DIRECTION
    {
        CW = 0,
        CCW,
    };

    /**
     * @brief 圆弧
     *
     */
    struct ARC
    {
        POINT c;
        double r;
        POINT st;
        POINT et;
        double sta;
        double spa;
        DIRECTION d;
        ARC(POINT c, double r, POINT st, POINT et, double sta, double spa, DIRECTION d)
        {
            this->c = c;
            this->r = r;
            this->st = st;
            this->et = et;
            this->sta = sta;
            this->spa = spa;
            this->d = d;
        }
    };
    /***************************************************************/

    /*************************点的基本运算***************************/

    /**
     * @brief 返回两点之间欧氏距离
     *
     * @param p1
     * @param p2
     * @return double
     */
    static double dist(POINT p1, POINT p2) { return (sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y))); }
    /***************************************************************/

    /*************************向量基本运算***************************/

    /**
     * @brief r=dotmultiply(p1,p2,op),得到矢量(p1-op)和(p2-op)的点积，如果两个矢量都非零矢量
     *        r<0：两矢量夹角为锐角
     *        r=0：两矢量夹角为直角
     *        r>0：两矢量夹角为钝角
     * @param p1
     * @param p2
     * @param p0
     * @return double
     */
    static double dotmultiply(POINT p1, POINT p2, POINT p0)
    {
        return ((p1.x - p0.x) * (p2.x - p0.x) + (p1.y - p0.y) * (p2.y - p0.y));
    }

    /**
     * @brief 矢量叉乘
     *        r=multiply(sp,ep,op),得到(sp-op)和(ep-op)的叉积
     *        r>0：ep在矢量opsp的逆时针方向；
     *        r=0：opspep三点共线；
     *        r<0：ep在矢量opsp的顺时针方向
     * @param sp
     * @param ep
     * @param op
     * @return double
     */
    static double multiply(POINT sp, POINT ep, POINT op)
    {
        return ((sp.x - op.x) * (ep.y - op.y) - (ep.x - op.x) * (sp.y - op.y));
    }
    /***************************************************************/

    /************************线段、直线基本运算***********************/

    /**
     * @brief 判断点与线段的关系,用途很广泛
     *        本函数是根据下面的公式写的，P是点C到线段AB所在直线的垂足
     *                       AC dot AB
     *        r =            ---------
     *                       ||AB||^2
     *             (Cx-Ax)(Bx-Ax) + (Cy-Ay)(By-Ay)
     *          = -------------------------------
     *                          L^2
     *        r 有下列含义:
     *          r=0    P = A
     *          r=1    P = B
     *          r<0    P 在AB的背面延伸
     *          r>1    P 在AB的前方延伸
     *          0<r<1  P 在AB范围内
     *
     * @param p
     * @param l
     * @return double
     */
    static double relation(POINT p, LINESEG l)
    {
        LINESEG tl;
        tl.s = l.s;
        tl.e = p;
        return dotmultiply(tl.e, l.e, l.s) / (dist(l.s, l.e) * dist(l.s, l.e));
    }

    /**
     * @brief 求点到线段所在直线垂线的垂足
     *
     * @param p
     * @param l
     * @return POINT
     */
    static POINT perpendicular(POINT p, LINESEG l)
    {
        double r = relation(p, l);
        POINT tp;
        tp.x = l.s.x + r * (l.e.x - l.s.x);
        tp.y = l.s.y + r * (l.e.y - l.s.y);
        return tp;
    }

    /**
     * @brief 求点p到线段l的最短距离,并返回线段上距该点最近的点np
              注意：np是线段l上到点p最近的点，不一定是垂足
     *
     * @param p
     * @param l
     * @param np
     * @return double
     */
    static double ptolinesegdist(POINT p, LINESEG l, POINT &np)
    {
        double r = relation(p, l);
        if (r < 0)
        {
            np = l.s;
            return dist(p, l.s);
        }
        if (r > 1)
        {
            np = l.e;
            return dist(p, l.e);
        }
        np = perpendicular(p, l);
        return dist(p, np);
    }

    /**
     * @brief 判断两线段是否相交(包括端点)
     *        判断P1P2跨立Q1Q2的依据是：( P1 - Q1 ) × ( Q2 - Q1 ) * ( Q2 - Q1 ) × ( P2 - Q1 ) >= 0
     *        判断Q1Q2跨立P1P2的依据是：( Q1 - P1 ) × ( P2 - P1 ) * ( P2 - P1 ) × ( Q2 - P1 ) >= 0
     * @param u
     * @param v
     * @return true 相交
     * @return false 不相交
     */
    static bool intersect(LINESEG u, LINESEG v)
    {
        return (
            (std::max(u.s.x, u.e.x) >= std::min(v.s.x, v.e.x)) &&                                                                                                                                                                         //排斥实验
            (std::max(v.s.x, v.e.x) >= std::min(u.s.x, u.e.x)) && (std::max(u.s.y, u.e.y) >= std::min(v.s.y, v.e.y)) && (std::max(v.s.y, v.e.y) >= std::min(u.s.y, u.e.y)) && (multiply(v.s, u.e, u.s) * multiply(u.e, v.e, u.s) >= 0) && //跨立实验
            (multiply(u.s, v.e, v.s) * multiply(v.e, u.e, v.s) >= 0));
    }

    /**
     * @brief 判断两条直线是否相交，相交则给出交点
     *        x = (b1 * c2 - b2 * c1) / (a1 * b2 - a2 * b1)
     *        y = (a2 * c1 - a1 * c2) / (a1 * b2 - a2 * b1)
     * @param a
     * @param b
     * @return true
     * @return false
     */
    static bool intersect(LINE u, LINE v, POINT &l)
    {
        bool res = u.a * v.b != v.a * u.b;
        if (res)
        {
            l.x = (u.b * v.c - v.b * u.c) / (u.a * v.b - v.a * u.b);
            l.y = (v.a * u.c - u.a * v.c) / (u.a * v.b - v.a * u.b);
        }
        return res;
    }

    /**
     * @brief 两点所在直线
     *        已知两个点A(x1,y1), B(x2,y2)时
     *        a = y2 - y1
     *        b = x1 - x2
     *        c = x2 * y1 - x1 * y2
     * @param a
     * @param b
     * @return LINE
     */
    static LINE ptoline(POINT a, POINT b) { return {b.y - a.y, a.x - b.x, b.x * a.y - a.x * b.y}; }

    /****************************************************************/

    /************************圆弧、圆基本运算***********************/

    /**
     * @brief 求点p到圆弧最近距离
     *        若圆心和p点组成的射线与圆弧有交点，则返回交点
     * @param p 某点
     * @param a 圆弧
     * @param np 某点距离圆弧最近的点
     * @return double
     */
    static double ptoarcsegdist(POINT p, ARC a, POINT &np)
    {
        // using namespace rededa::Vector;
        // LINESEG se(a.st, a.et);
        // double spdist = dist(a.st, p);
        // double epdist = dist(a.et, p);
        // if (intersect(ptoline(a.c, p), ptoline(a.st, a.et), np))
        // {
        //     auto r = relation(np, se);
        //     if (r >= 0 && r <= 1 && dotmultiply(p, np, a.c) > 0)
        //     {
        //         auto cp(p - a.c);
        //         auto normalCP = normalized(cp);
        //         np = a.r * normalCP + a.c;
        //         return std::abs(module(cp) - a.r);
        //     }
        // }
        // np = spdist < epdist ? a.st : a.et;
        // return std::min(spdist, epdist);
    }

    /**
     * @brief 判断点是否在圆内(包括边界)
     *        因为圆为凸集，所以判断点集，折线，多边形是否在圆内时，
     *        只需要逐一判断点是否在圆内即可。
     * @param o 目标点
     * @param r 半径
     * @param p 圆心
     * @return true  在圆内
     * @return false 不在圆内
     */
    static bool pincircle(POINT o, double r, POINT p)
    {
        double d2 = (p.x - o.x) * (p.x - o.x) + (p.y - o.y) * (p.y - o.y);
        double r2 = r * r;
        return d2 < r2 || abs(d2 - r2) < EP;
    }

    /**
     * @brief 判断两圆关系
     *
     * @param p1 圆1圆心
     * @param r1 圆1半径
     * @param p2 圆2圆心
     * @param r2 圆2半径
     * @return 1 相离
     * @return 2 外切
     * @return 3 相交
     * @return 4 内切
     * @return 5 内含
     */
    static int circlerelation(POINT p1, double r1, POINT p2, double r2)
    {
        double d = sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));

        if (fabs(d - r1 - r2) < EP) // 必须保证前两个if先被判定！
            return 2;
        if (fabs(d - fabs(r1 - r2)) < EP)
            return 4;
        if (d > r1 + r2)
            return 1;
        if (d < fabs(r1 - r2))
            return 5;
        if (fabs(r1 - r2) < d && d < r1 + r2)
            return 3;
        return 0; // indicate an error!
    }
    /****************************************************************/

} // namespace geometry

#endif // REDEDA_GEOMETRY_ALGO_H
