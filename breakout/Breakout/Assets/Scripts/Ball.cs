using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Ball : MonoBehaviour
{

    public Rigidbody2D body;
    public float speed = 500f;
    private void Awake()
    {
        this.body = GetComponent<Rigidbody2D>();
    }

    private void Start()
    {
        Invoke(nameof(startGame), 1f);
    }

    public void restart()
    {
        Start();
    }
    private void startGame()
    {
        Vector2 force = Vector2.zero;
        force.x = Random.Range(-1f, 1f);
        force.y = 1f;

        this.body.AddForce(force.normalized * this.speed);
    }
}
