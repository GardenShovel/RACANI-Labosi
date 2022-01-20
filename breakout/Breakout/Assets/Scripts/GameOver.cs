using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GameOver : MonoBehaviour
{
    Brick[] bricks;
    [SerializeField]
    Ball ball;
    [SerializeField]
    Paddle paddle;

    private void Start()
    {
        bricks = FindObjectsOfType<Brick>();
    }
    private void OnCollisionEnter2D(Collision2D collision)
    {
        ball.body.velocity = Vector2.zero;
        Invoke(nameof(Restart), 1);
    }

    private void Restart()
    {
        for (int i = 0; i < bricks.Length; i++)
        {
            bricks[i].gameObject.SetActive(true);
        }
        ball.transform.position = new Vector2(0, -4);
        ball.body.velocity = Vector2.zero;
        paddle.body.velocity = Vector2.zero;
        paddle.transform.position = new Vector2(0, -4.57f);
        ball.restart();
    }
}
